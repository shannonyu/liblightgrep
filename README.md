liblightgrep
============

not the worst forensics regexp engine

About
-----
Lightgrep is a new regular expression engine, designed specifically for digital forensics. Why another regexp engine?

Lightgrep:
 * searches for many patterns simultaneously
 * searches binary data as a stream, not as discrete lines of text
 * searches for patterns in many different encodings; give it dirty data, lightgrep don't care
 * never, ever, ever, never, never looks at a byte twice or backs up in your input

Lightgrep is still pretty new and doesn't have all the regexp features you might be used to. But it has enough features to be more than a toy, and what is supported is well-tested.

liblightgrep is copyright (c) 2010-2012, Lightbox Technologies, Inc. liblightgrep is available under version 3 of the GNU Public License. See LICENSE for details.

Technical Info
--------------
[![Build Status](https://secure.travis-ci.org/jonstewart/liblightgrep.png?branch=master)](https://travis-ci.org/jonstewart/liblightgrep)

Lightgrep is implemented in portable C++11 but exposes a concise C API. It depends on a number of [Boost](http://www.boost.org/) libraries and also on [ICU](http://www.icu-project.org). Currently you will need gcc 4.6+ or clang 3.1 to compile the libraries.

Install
-------
See [[BUILD.md]] for installation instructions.
