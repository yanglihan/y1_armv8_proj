// memory access
#ifndef MEMORY_H
#define MEMORY_H

#include "../common/datatypes.h"
#include <stdio.h>

#define MEM_SIZE (0x200000)   // 2MB of memory

// load memory from a file input
extern int memfread(FILE *in);

// load a 64-bit data at addr
extern uint64_t mload64(addr_t addr);

// store a 64-bit data at addr
extern void mstore64(addr_t addr, uint64_t data);

// load a 32-bit data from addr
extern uint32_t mload32(addr_t addr);

// store a 32-bit data at addr
extern void mstore32(addr_t addr, uint32_t data);

#endif
