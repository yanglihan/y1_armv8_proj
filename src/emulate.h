#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MEM_SIZE 0x200000   // 2MB of memory
#define UPPER half[1]       // to access upper half of a 64-bit register

typedef union
{
  int64_t x;
  int32_t w;
  uint64_t ux;
  uint32_t uw;
  uint32_t half[2];
} reg_t;

typedef uint32_t instr_t;   // 32-bit instruction
typedef uint32_t seg_t;     // segment of an instruction
typedef uint64_t addr_t;    // memory address

uint64_t mem64_load(addr_t addr);

void mem64_store(addr_t addr, uint64_t data);

uint32_t mem32_load(addr_t addr);

void mem32_store(addr_t addr, uint32_t data);

inline uint64_t take_bits(void *from, int begin, int size);

uint32_t bit_shift32(seg_t opr, seg_t oprand, int shift_amount);

uint64_t bit_shift64(seg_t opr, uint64_t oprand, int shift_amount);

void dpi(instr_t instr);

void ls(instr_t instr);

void br(instr_t instr);

void print_state();
