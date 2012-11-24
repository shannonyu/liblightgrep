/*
  liblightgrep: not the worst forensics regexp engine
  Copyright (C) 2012 Lightbox Technologies, Inc

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ascii.h"
#include "concrete_encoders.h"
#include "encoderfactory.h"
#include "lightgrep/lightgrep_c_util.h"
#include "lightgrep/transforms.h"
#include "lightgrep/encodings.h"
#include "icuencoder.h"
#include "oceencoder.h"
#include "rotencoder.h"
#include "utf8.h"
#include "utf16.h"
#include "utf32.h"
#include "xorencoder.h"

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

EncoderFactory::EncoderFactory():
  Cache{
    { "ASCII",    std::make_shared<ASCII>()          },
    { "UTF-8",    std::make_shared<CachingUTF8>()    },
    { "UTF-16LE", std::make_shared<CachingUTF16LE>() },
    { "UTF-16BE", std::make_shared<CachingUTF16BE>() },
    { "UTF-32LE", std::make_shared<CachingUTF32LE>() },
    { "UTF-32BE", std::make_shared<CachingUTF32BE>() }
  }
{}

std::shared_ptr<Encoder> EncoderFactory::get(const std::string& chain) {
// TODO: extract this out to a yacc grammar

  // return a cached encoder if we have one
  auto i = Cache.find(chain);
  if (i != Cache.end()) {
    return i->second;
  }

  typedef boost::char_separator<char> char_separator;
  typedef boost::tokenizer<char_separator> tokenizer;

  const tokenizer tokens(chain, char_separator("|"));
  tokenizer::const_iterator curTok(tokens.begin());
  const tokenizer::const_iterator endTok(tokens.end());

  int id;

  // process the char->char transformations
  std::vector<std::string> charchar;
  for ( ; curTok != endTok; ++curTok) {
    id = lg_get_char_char_transformation_id(curTok->c_str());
    if (id < 0) {
      break;
    }

    charchar.emplace_back(LG_CANONICAL_CHAR_CHAR_TRANSFORMATIONS[id]);
  }

  if (curTok == endTok) {
    THROW_RUNTIME_ERROR_WITH_OUTPUT("No valid char->byte transformation");
  }

  // process the char->byte transformation
  id = lg_get_encoding_id(curTok->c_str());
  if (id < 0) {
    THROW_RUNTIME_ERROR_WITH_OUTPUT(
      "'" << *curTok << "' is neither a valid char->char transformation "
      "nor a valid char->byte transformation"
    );
  }

  const std::string charbyte = LG_CANONICAL_ENCODINGS[id];
  ++curTok;

  // process the byte->byte transformations
  std::vector<std::string> bytebyte;
  for ( ; curTok != endTok; ++curTok) {
    id = lg_get_byte_byte_transformation_id(curTok->c_str());
    if (id < 0) {
      THROW_RUNTIME_ERROR_WITH_OUTPUT(
        "'" << *curTok << "' is not a valid byte->byte transformation"
      );
    }

    bytebyte.emplace_back(LG_CANONICAL_BYTE_BYTE_TRANSFORMATIONS[id]);
  }

  // assemble the transformation chain 
  
  std::unique_ptr<Encoder> enc;

  if (charbyte == "ASCII") {
    enc.reset(new ASCII);
  }
  else if (charbyte == "UTF-8") {
    enc.reset(new UTF8);
  }
  else if (charbyte == "UTF-16LE") {
    enc.reset(new UTF16LE);
  }
  else if (charbyte == "UTF-16BE") {
    enc.reset(new UTF16BE);
  }
  else if (charbyte == "UTF-32LE") {
    enc.reset(new UTF32LE);
  }
  else if (charbyte == "UTF-32BE") {
    enc.reset(new UTF32BE);
  }
  else {
    enc.reset(new ICUEncoder(charbyte));
  }

  for (const std::string& bb : bytebyte) {
    if (bb == "identity") {
      // do nothing
    }
    else if (bb == "OCE") {
      enc.reset(new OCEEncoder(std::move(enc)));
    }
  }

  for (auto cc = charchar.crbegin(); cc != charchar.crend(); ++cc) {
    if (*cc == "identity") {
      // do nothing
    }
    else if (cc->substr(0, 3) == "rot") {
      enc.reset(new ROTEncoder(
        boost::lexical_cast<uint32_t>(cc->substr(3)), std::move(enc)
      ));
    }
    else if (cc->substr(0, 3) == "XOR") {
      enc.reset(new XOREncoder(
        boost::lexical_cast<byte>(cc->substr(3)), std::move(enc)
      ));
    }
  }

  return std::shared_ptr<Encoder>(new CachingEncoder(std::move(enc)));
}
