// bitwise operations
#ifndef BITWISE_H
#define BITWISE_H

#include "../common/datatypes.h"

#define sgnext64(opr, by)  (int64_t)((uint64_t)opr << by) >> by
#define sgnext32(opr, by)  (int32_t)((uint32_t)opr << by) >> by

// take a segment of a binary starting from begin of size size
extern uint64_t bitfield(void* from, int begin, int size);

// shift 32-bit operand by n under the mode described in shift
extern uint32_t bitshift32(seg_t shift, seg_t operand, int n);

// shift 64-bit operand by n under the mode described in shift
extern uint64_t bitshift64(seg_t shift, uint64_t operand, int n);

#endif
