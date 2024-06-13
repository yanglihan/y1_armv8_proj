#include "bitwise.h"
#include "datatype.h"

// take a segment of a binary starting from begin of size size
inline uint64_t take_bits(void *from, int begin, int size)
{
  uint64_t i = *(uint64_t *)from;
  i >>= begin;
  i &= (1 << size) - 1;
  return i;
}

// shift 32-bit operand by shift_amount under the mode described in opr
uint32_t bit_shift32(seg_t opr, seg_t operand, int shift_amount)
{
  switch (opr & 0b0110)
  {
  case 0b0000: // lsl
    return operand << shift_amount;
    break;
  case 0b0010: // lsr
    return operand >> shift_amount;
    break;
  case 0b0100: // asr
    return (operand >> shift_amount) | (take_bits(&operand, 31, 1) * (UINT32_MAX << (32 - shift_amount)));
    break;
  case 0b0110: // ror
    return (operand >> shift_amount) | (operand << (32 - shift_amount));
    break;
  }
  return 0;
}

// shift 64-bit operand by shift_amount under the mode described in opr
uint64_t bit_shift64(seg_t opr, uint64_t operand, int shift_amount)
{
  switch (opr & 0b0110)
  {
  case 0b0000: // lsl
    return operand << shift_amount;
    break;
  case 0b0010: // lsr
    return operand >> shift_amount;
    break;
  case 0b0100: // asr
    return (operand >> shift_amount) | (take_bits(&operand, 63, 1) * (UINT64_MAX << (64 - shift_amount)));
    break;
  case 0b0110: // ror
    return (operand >> shift_amount) | (operand << (64 - shift_amount));
    break;
  }
  return 0;
}
