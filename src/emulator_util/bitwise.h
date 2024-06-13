
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datatypes.h"

#ifndef BITWISE
#define BITWISE

// take a segment of a binary starting from begin of size size
extern inline uint64_t take_bits(void *from, int begin, int size);

// shift 32-bit operand by shift_amount under the mode described in opr
extern uint32_t bit_shift32(seg_t opr, seg_t operand, int shift_amount);

// shift 64-bit operand by shift_amount under the mode described in opr
extern uint64_t bit_shift64(seg_t opr, uint64_t operand, int shift_amount);

#endif
