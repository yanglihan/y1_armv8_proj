// stores the state of armv8
#ifndef STATE_H
#define STATE_H

#include "datatypes.h"
#include "memory.h"

#define PC          (pc.uw)             // uniformed access to program counter
#define PC_INC      pc.uw += 4          // program counter increment
#define CUR_INSTR   (mem32_load(pc.ux)) // current instruction
#define RESET_ZR    zr->ux = 0          // reset zero register

// registers
extern reg_t r[32], * zr, pc;

// p-states
extern pstate_t pstate;

#endif
