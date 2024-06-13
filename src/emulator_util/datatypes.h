// datatypes related to armv8
#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdint.h>

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define UPPER uhalf[0]       // to access upper half of a 64-bit register
#define w     half[1]
#define uw    uhalf[1]
#else
#define UPPER uhalf[1]       // to access upper half of a 64-bit register
#define w     half[0]
#define uw    uhalf[0]
#endif

// pstate
typedef struct
{
  unsigned int n : 1, z : 1, c : 1, v : 1;
} pstate_t;

// general register
typedef union reg
{
  int64_t     x;
  // int32_t  w; implemented as a macro using half
  uint64_t    ux;
  // uint32_t uw; implemented as a macro using uhalf
  int32_t     half[2];
  uint32_t    uhalf[2];
} reg_t;

typedef uint32_t instr_t;   // 32-bit instruction
typedef uint64_t seg_t;     // segment of an instruction
typedef uint64_t addr_t;    // memory address

#endif
