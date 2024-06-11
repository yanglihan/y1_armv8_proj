#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
  // int32_t  w; this is implemented as a macro using half
  uint64_t    ux;
  // uint32_t uw; this is implemented as a macro using uhalf
  int32_t     half[2];
  uint32_t    uhalf[2];
} reg_t;

typedef uint32_t instr_t;   // 32-bit instruction
typedef uint32_t seg_t;     // segment of an instruction
typedef uint64_t addr_t;    // memory address

// load a 64-bit data at addr
extern uint64_t mem64_load(addr_t addr);

// store a 64-bit data at addr
extern void mem64_store(addr_t addr, uint64_t data);

// load a 32-bit data from addr
extern uint32_t mem32_load(addr_t addr);

// store a 32-bit data at addr
extern void mem32_store(addr_t addr, uint32_t data);

// take a segment of a binary starting from begin of size size
extern inline uint64_t take_bits(void *from, int begin, int size);

// shift 32-bit oprand by shift_amount under the mode described in opr
extern uint32_t bit_shift32(seg_t opr, seg_t oprand, int shift_amount);

// shift 64-bit oprand by shift_amount under the mode described in opr
extern uint64_t bit_shift64(seg_t opr, uint64_t oprand, int shift_amount);

// data processing (immediate)
extern void dpi(instr_t instr);

// data processing (register)
extern void dpr(instr_t instr);

// loads and stores
extern void ls(instr_t instr);

// branches
extern void br(instr_t instr);

// print all registers and non-zero memories
extern void print_state();
