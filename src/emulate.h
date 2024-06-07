#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MEM_SIZE 0x200000 // 2MB of memory
uint8_t mem[MEM_SIZE];

union reg
{
  int64_t x;
  int32_t w;
  uint64_t ux;
  uint32_t uw;
  uint32_t upper[2];
};

typedef uint32_t instr;
typedef uint32_t seg;

uint64_t mem64_load(uint64_t addr);

void mem64_store(uint64_t addr, uint64_t data);

uint32_t mem32_load(uint64_t addr);

void mem32_store(uint64_t addr, uint32_t data);

inline uint64_t take_bits(void *from, int begin, int size);

uint32_t bit_shift32(seg opr, seg oprand, int shift_amount);

uint64_t bit_shift64(seg opr, uint64_t oprand, int shift_amount);

void dpi(instr instr);

void ls(instr instr);

void br(instr instr);

void print_state();
