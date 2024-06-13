#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "datatypes.h"
#include "bitwise.h"

// load a 64-bit data at addr
extern uint64_t mem64_load(addr_t addr);

// store a 64-bit data at addr
extern void mem64_store(addr_t addr, uint64_t data);

// load a 32-bit data from addr
extern uint32_t mem32_load(addr_t addr);

// store a 32-bit data at addr
extern void mem32_store(addr_t addr, uint32_t data);

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

// print all registers and non-zero memories
extern void fprint_state(FILE *out);
