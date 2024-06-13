// an interpreter for binary instructions
#ifndef INTERPRET_H
#define INTERPRET_H

#include "op_dpi.h"
#include "op_dpr.h"
#include "op_ls.h"
#include "op_br.h"
#include "datatypes.h"

#define INSTR_HALT(instr)   (instr == 0x8a000000)
#define OP0_DPI(op0)        ((op0 & 0b1110) == 0b1000)
#define OP0_DPR(op0)        ((op0 & 0b0111) == 0b0101)
#define OP0_LS(op0)         ((op0 & 0b0101) == 0b0100)
#define OP0_BR(op0)         ((op0 & 0b1110) == 0b1010)

extern int interpret(instr_t instr);

#endif
