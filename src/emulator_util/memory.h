// memory access
#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>

#include "datatypes.h"

#define MEM_SIZE (0x200000)   // 2MB of memory

#define load_mem(in) fread(mem, MEM_SIZE, 1, in)

extern uint8_t mem[MEM_SIZE];

// load a 64-bit data at addr
extern uint64_t mem64_load(addr_t addr);

// store a 64-bit data at addr
extern void mem64_store(addr_t addr, uint64_t data);

// load a 32-bit data from addr
extern uint32_t mem32_load(addr_t addr);

// store a 32-bit data at addr
extern void mem32_store(addr_t addr, uint32_t data);

#endif
