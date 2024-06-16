// stores the state of armv8
#ifndef STATE_H
#define STATE_H

#include "memory.h"
#include "../common/datatypes.h"

#define PC          (pc.uw)             // uniformed access to program counter
#define PC_INC      pc.uw += 4          // program counter increment
#define CUR_INSTR   (mload32(pc.ux))    // current instruction
#define RESET_ZR    zr->ux = 0          // reset zero register

#define P_FLG_N     (pstate.n ? 'N' : '-')
#define P_FLG_Z     (pstate.z ? 'Z' : '-')
#define P_FLG_C     (pstate.c ? 'C' : '-')
#define P_FLG_V     (pstate.v ? 'V' : '-')

// registers
extern reg_t r[32], * zr, pc;

// p-states
extern pstate_t pstate;

#endif
