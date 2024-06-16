#include "memory.h"

uint8_t mem[MEM_SIZE]; // memory

// load a 64-bit data at addr
uint64_t mload64(addr_t addr)
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
void mstore64(addr_t addr, uint64_t data)
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
uint32_t mload32(addr_t addr)
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
void mstore32(addr_t addr, uint32_t data)
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

// load memory from a file input
int memfread(FILE *in)
{
  return fread(mem, MEM_SIZE, 1, in);
}
