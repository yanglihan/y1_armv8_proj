#include "op_br.h"

// branches
void br(instr_t instr)
{
  seg_t simm26 = take_bits(&instr, 0, 26);
  seg_t simm19 = take_bits(&instr, 5, 19);
  int64_t offset;
  seg_t cond;
  int8_t exec;
  switch (take_bits(&instr, 30, 2))
  {
  case BR_UNCOND: // unconditional
    offset = sgnext64(simm26, 38);
    PC += offset * 4; // jump to specified offset
    break;

  case BR_COND: // conditional
    offset = sgnext64(simm19, 45);
    cond = take_bits(&instr, 0, 4);
    switch (cond)
    {
    case BR_COND_EQ: // eq
      exec = pstate.z == 1;
      break;
    case BR_COND_NE: // ne
      exec = pstate.z != 1;
      break;
    case BR_COND_GE: // ge
      exec = pstate.n == pstate.v;
      break;
    case BR_COND_LT: // lt
      exec = pstate.n != pstate.v;
      break;
    case BR_COND_GT: // gt
      exec = !pstate.z && pstate.n == pstate.v;
      break;
    case BR_COND_LE: // le
      exec = pstate.z || pstate.n != pstate.v;
      break;
    case BR_COND_AL: // al
      exec = 1;
      break;
    default:
      exec = 0;
    }
    if (exec) // condition fulfilled
    {
      PC += offset * 4; // jump to specified offset
    }
    else // condition failed
    {
      PC_INC; // update program counter
    }
    break;

  case BR_REG: // register
  {
    seg_t xn = take_bits(&instr, 5, 5);
    PC = (r + xn)->uw; // jump to address stored in register
  }
  break;
  }
  // program counter not updated here as all branches leads to their own update mode
}
