// branch operation
#ifndef OP_BR_H
#define OP_BR_H

#include "datatypes.h"
#include "state.h"
#include "bitwise.h"

#define BR_COND_EQ  (0b0000)
#define BR_COND_NE  (0b0001)
#define BR_COND_GE  (0b1010)
#define BR_COND_LT  (0b1011)
#define BR_COND_GT  (0b1100)
#define BR_COND_LE  (0b1101)
#define BR_COND_AL  (0b1110)

#define BR_UNCOND   (0b00)
#define BR_COND     (0b01)
#define BR_REG      (0b11)

// branch
extern void br(instr_t instr);

#endif
