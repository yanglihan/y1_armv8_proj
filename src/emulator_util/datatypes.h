#include <stdint.h>

#ifndef DATATYPES
#define DATATYPES

#define MEM_SIZE (0x200000)   // 2MB of memory
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define UPPER uhalf[0]       // to access upper half of a 64-bit register
#define w     half[1]
#define uw    uhalf[1]
#else
#define UPPER uhalf[1]       // to access upper half of a 64-bit register
#define w     half[0]
#define uw    uhalf[0]
#endif

typedef union
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
