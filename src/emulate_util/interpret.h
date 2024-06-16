// an interpreter for binary instructions
#ifndef INTERPRET_H
#define INTERPRET_H

#include "../common/datatypes.h"

#define C_INSTR_HALT(instr)     (instr == 0x8a000000)
#define C_OP0_DPI(op0)          ((op0 & 0b1110) == 0b1000)
#define C_OP0_DPR(op0)          ((op0 & 0b0111) == 0b0101)
#define C_OP0_LS(op0)           ((op0 & 0b0101) == 0b0100)
#define C_OP0_BR(op0)           ((op0 & 0b1110) == 0b1010)

// interpret an instruction
extern int interpret(instr_t instr);

#endif
