#pragma once

#include "basic.h"

enum OpCodes {
  UNINITIALIZED = 0,
  LIT_OP,
  EITHER_OP,
  RANGE_OP,
  BIT_VECTOR_OP,
  JUMP_TABLE_OP,
  JUMP_TABLE_RANGE_OP,
  JUMP_OP,
  LONGJUMP_OP,
  FORK_OP,
  LONGFORK_OP,
  CHECK_HALT_OP,
  LABEL_OP,
  MATCH_OP,
  HALT_OP,
  ILLEGAL
};

#pragma pack(1)
struct ByteRange {
  byte First, Last;
};

union Operand {
  unsigned  Offset : 24;
  ByteRange Range;
  byte      Literal;
};

struct Instruction {
  signed   OpCode : 6;
  unsigned Size   : 2;
  Operand  Op;

  Instruction(): OpCode(ILLEGAL), Size(0) { Op.Offset = 0; }

  byte wordSize() const {
    switch (Size) {
      case 1:
        return 2; // 4 bytes of data after this instruction
      case 2:
        return 5; // 16 bytes of data after this instruction
      case 3:
        return 9; // 32 bytes of data after this instruction
      default:
        return 1; // no additional data after this instruction
    }
  }

  byte byteSize() const { return sizeof(Instruction) * wordSize(); }

  std::string toString() const;

  bool operator==(const Instruction& x) const { return *((uint32*)this) == *((uint32*)&x); } // total hack

  static Instruction makeLit(byte b);
  static Instruction makeEither(byte one, byte two);
  static Instruction makeRange(byte first, byte last);
  static Instruction makeBitVector();
  static Instruction makeJump(uint32 relativeOffset);
  static Instruction makeLongJump(Instruction* ptr, uint32 relativeOffset);
  static Instruction makeJumpTable();
  static Instruction makeJumpTableRange(byte first, byte last);
  static Instruction makeLabel(uint32 label);
  static Instruction makeMatch();
  static Instruction makeFork(uint32 index);
  static Instruction makeLongFork(Instruction* ptr, uint32 relativeOffset);
  static Instruction makeCheckHalt(uint32 checkIndex);
  static Instruction makeHalt();
};
#pragma pack()

std::ostream& operator<<(std::ostream& out, const Instruction& instr);
