#include "memory.h"

uint8_t mem[MEM_SIZE]; // memory

// load a 64-bit data at addr
uint64_t mem64_load(addr_t addr)
{
  if (addr + 7 >= MEM_SIZE)
  {
    return 0;
  }
  uint64_t value = 0;
  for (int i = 0; i < 8; i++)
  {
    value |= ((uint64_t)mem[addr + i]) << (8 * i);
  }
  return value;
}

// store a 64-bit data at addr
void mem64_store(addr_t addr, uint64_t data)
{
  if (addr + 7 >= MEM_SIZE)
  {
    return;
  }
  for (int i = 0; i < 8; i++)
  {
    mem[addr + i] = (data >> (8 * i)) & 0xFF;
  }
}

// load a 32-bit data from addr
uint32_t mem32_load(addr_t addr)
{
  if (addr + 3 >= MEM_SIZE)
  {
    return 0;
  }
  uint32_t value = 0;
  for (int i = 0; i < 4; i++)
  {
    value |= ((uint32_t)mem[addr + i]) << (8 * i);
  }
  return value;
}

// store a 32-bit data at addr
void mem32_store(addr_t addr, uint32_t data)
{
  if (addr + 3 >= MEM_SIZE)
  {
    return;
  }
  for (int i = 0; i < 4; i++)
  {
    mem[addr + i] = (data >> (8 * i)) & 0xFF;
  }
}
