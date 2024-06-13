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
  case 0b00: // unconditional
    offset = (int64_t)((uint64_t)simm26 << 38) >> 38; // sign ext
    PC += offset * 4; // jump to specified offset
    break;

  case 0b01: // conditional
    offset = (int64_t)((uint64_t)simm19 << 45) >> 45; // sign ext
    cond = take_bits(&instr, 0, 4);
    exec = 0;
    switch (cond)
    {
    case 0b0000: // eq
      exec = pstate.z == 1;
      break;
    case 0b0001: // ne
      exec = pstate.z != 1;
      break;
    case 0b1010: // ge
      exec = pstate.n == pstate.v;
      break;
    case 0b1011: // lt
      exec = pstate.n != pstate.v;
      break;
    case 0b1100: // gt
      exec = !pstate.z && pstate.n == pstate.v;
      break;
    case 0b1101: // le
      exec = pstate.z || pstate.n != pstate.v;
      break;
    case 0b1110: // al
      exec = 1;
      break;
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

  case 0b11: // register
  {
    seg_t xn = take_bits(&instr, 5, 5);
    PC = (r + xn)->uw; // jump to address stored in register
  }
  break;
  }
  // program counter not updated here as all branches leads to their own update mode
}
