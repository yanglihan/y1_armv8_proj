#include "bitwise.h"

#include "../common/consts.h"

// take a segment of a binary starting from begin of size size
uint64_t bitfield(void* from, int begin, int size)
{
  uint64_t i = *(uint64_t*)from;
  i >>= begin;
  i &= (1 << size) - 1;
  return i;
}

// shift 32-bit operand by n under the mode described in shift
uint32_t bitshift32(seg_t shift, seg_t operand, int n)
{
  switch (shift)
  {
  case SHIFT_LSL: // lsl
    return operand << n;
    break;
  case SHIFT_LSR: // lsr
    return operand >> n;
    break;
  case SHIFT_ASR: // asr
    return (operand >> n) | (bitfield(&operand, 31, 1) * (UINT32_MAX << (32 - n)));
    break;
  case SHIFT_ROR: // ror
    return (operand >> n) | (operand << (32 - n));
    break;
  }
  return 0;
}

// shift 64-bit operand by n under the mode described in shift
uint64_t bitshift64(seg_t shift, uint64_t operand, int n)
{
  switch (shift)
  {
  case SHIFT_LSL: // lsl
    return operand << n;
    break;
  case SHIFT_LSR: // lsr
    return operand >> n;
    break;
  case SHIFT_ASR: // asr
    return (operand >> n) | (bitfield(&operand, 63, 1) * (UINT64_MAX << (64 - n)));
    break;
  case SHIFT_ROR: // ror
    return (operand >> n) | (operand << (64 - n));
    break;
  }
  return 0;
}
