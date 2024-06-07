
#include "emulate.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_SIZE 0x200000 // 2MB of memory
uint8_t mem[MEM_SIZE];

union reg r[32], *zr = r + 31, pc, sp;
typedef uint32_t instr;
typedef uint32_t seg;

struct
{
  bool n, z, c, v;
} pstate = {0, 1, 0, 0};

uint64_t mem64_load(uint64_t addr)
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

void mem64_store(uint64_t addr, uint64_t data)
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

uint32_t mem32_load(uint64_t addr)
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

void mem32_store(uint64_t addr, uint32_t data)
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

uint64_t take_bits(void *from, int begin, int size)
{
  uint64_t i = *(uint64_t *)from;
  i >>= begin;
  i &= (1 << size) - 1;
  return i;
}

uint32_t bit_shift32(seg opr, seg oprand, int shift_amount)
{
  switch (opr & 0b0110)
  {
  case 0b0000: // lsl
    return oprand << shift_amount;
    break;
  case 0b0010: // lsr
    return oprand >> shift_amount;
    break;
  case 0b0100: // asr
    return (oprand >> shift_amount) | (take_bits(&oprand, 31, 1) * (UINT32_MAX << (32 - shift_amount)));
    break;
  case 0b0110: // ror
    return (oprand >> shift_amount) | (oprand << (32 - shift_amount));
    break;
  }
  return 0;
}

uint64_t bit_shift64(seg opr, uint64_t oprand, int shift_amount)
{
  switch (opr & 0b0110)
  {
  case 0b0000: // lsl
    return oprand << shift_amount;
    break;
  case 0b0010: // lsr
    return oprand >> shift_amount;
    break;
  case 0b0100: // asr
    return (oprand >> shift_amount) | (take_bits(&oprand, 63, 1) * (UINT64_MAX << (64 - shift_amount)));
    break;
  case 0b0110: // ror
    return (oprand >> shift_amount) | (oprand << (64 - shift_amount));
    break;
  }
  return 0;
}

void dpi(instr instr)
{
  seg sf = take_bits(&instr, 31, 1);
  seg opc = take_bits(&instr, 29, 2);
  seg opi = take_bits(&instr, 23, 3);
  seg operand = take_bits(&instr, 5, 18);
  seg rdi = take_bits(&instr, 0, 5);
  union reg *rd = r + rdi;
  seg sh = take_bits(&instr, 22, 1);
  int64_t imm12 = take_bits(&instr, 10, 12) << (sh * 12);
  seg rni = take_bits(&instr, 5, 5);
  union reg *rn = r + rni;
  seg hw = take_bits(&instr, 21, 2);
  int64_t imm16 = take_bits(&instr, 5, 16) << (hw * 16);

  switch (opi)
  {
  case 0b010: // arithmatic operation
    switch (opc)
    {
    case 0b00: // add
      if (sf)  // 64-bit
      {
        rd->x = rn->x + (int64_t)imm12;
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->w = rn->w + (int32_t)imm12;
      }
      break;
    case 0b01: // adds
      if (sf)  // 64-bit
      {
        rd->x = rn->x + (int64_t)imm12;
        pstate.n = take_bits(rd, 63, 1);
        pstate.z = (rd->ux == 0);
        pstate.c = (UINT64_MAX - imm12 > rn->ux);
        pstate.v = (INT64_MAX - (int64_t)imm12 > rn->x);
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->w = rn->w + (int32_t)imm12;
        pstate.n = take_bits(rd, 31, 1);
        pstate.z = (rd->uw == 0);
        pstate.c = (UINT32_MAX - imm12 > rn->uw);
        pstate.v = (INT32_MAX - (int32_t)imm12 > rn->w);
      }
      break;
    case 0b10: // sub
      if (sf)  // 64-bit
      {
        rd->x = rn->x - (int64_t)imm12;
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->w = rn->w - (int32_t)imm12;
      }
      break;
    case 0b11: // subs
      if (sf)  // 64-bit
      {
        rd->x = rn->x - (int64_t)imm12;
        pstate.n = take_bits(rd, 63, 1);
        pstate.z = (rd->ux == 0);
        pstate.c = (imm12 <= rn->ux);
        pstate.v = (INT64_MIN + (int64_t)imm12 > rn->x);
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->w = rn->w - (int32_t)imm12;
        pstate.n = take_bits(rd, 31, 1);
        pstate.z = (rd->uw == 0);
        pstate.c = (imm12 <= rn->uw);
        pstate.v = (INT32_MIN + (int32_t)imm12 > rn->w);
      }
      break;
    }
    break;
  case 0b101: // wide move operation
    switch (opc)
    {
    case 0b00: // movn
      if (sf)  // 64-bit
      {
        rd->ux = ~(uint64_t)imm16;
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->uw = ~(uint32_t)imm16;
      }
      break;
    case 0b10: // movz
      if (sf)  // 64-bit
      {
        rd->ux = (uint64_t)imm16;
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->uw = (uint32_t)imm16;
      }
      break;
    case 0b11: // movk
      if (sf)  // 64-bit
      {
        //printf("%x %x\n",rd->ux, imm16);
        rd->ux = (uint64_t)imm16 | (rd->ux & ~((uint64_t)0xffff << (hw * 16)));
        //printf("%x %x\n",rd->ux, imm16);
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->uw = (uint32_t)imm16 | (rd->uw & ~((uint32_t)0xffff << (hw * 16)));
      }
      break;
    }
    break;
  }
  zr->uw = 0;
}

void dpr(instr instr)
{
  seg sf = take_bits(&instr, 31, 1);
  seg opc = take_bits(&instr, 29, 2);
  seg m = take_bits(&instr, 28, 1);
  seg opr = take_bits(&instr, 21, 4);
  seg rmi = take_bits(&instr, 16, 5); 
  seg operand = take_bits(&instr, 10, 6);
  seg rni = take_bits(&instr, 5, 5);
  seg rdi = take_bits(&instr, 0, 5);
  seg rai = take_bits(&instr, 10, 5);
  union reg *rm = r + rmi;
  union reg *rn = r + rni;
  union reg *rd = r + rdi;
  union reg *ra = r + rai;
  int64_t imm;

  if (m)
  {
    if (opr == 0b1000) // multiply
    {
      if (!(operand & 0b100000)) // madd
      {
        if (sf) // 64-bit
        {
          rd->x = ra->x + rn->x * rm->x;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = ra->w + rn->w * rm->w;
        }
      }
      else // msub
      {
        if (sf) // 64-bit
        {
          rd->x = ra->x - rn->x * rm->x;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = ra->w - rn->w * rm->w;
        }
      }
    }
  }
  else
  {
    if ((opr & 0b1001) == 0b1000) // arithmatic
    {
      if (sf) // 64-bit
      {
        imm = bit_shift64(opr, rm->x, operand);
      }
      else // 32-bit
      {
        imm = bit_shift32(opr, rm->w, operand);
      }
      switch (opc)
      {
      case 0b00: // add
        if (sf)  // 64-bit
        {
          rd->x = rn->x + (int64_t)imm;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = rn->w + (int32_t)imm;
        }
        break;
      case 0b01: // adds
        if (sf)  // 64-bit
        {
          uint64_t result = rn->x + imm;
          rd->x = result;
          pstate.n = (result >> 63) & 1; 
          pstate.z = (result == 0); 
          pstate.c = result < rn->x; 
          pstate.v = (((rn->x < 0) == (imm < 0)) && ((result < 0) != (rn->x < 0))); 
        }
        else // 32-bit
        {
          uint32_t result = rn->w + (int32_t)imm;
          rd->w = result;
          rd->upper[1] = 0; 
          pstate.n = (result >> 31) & 1; 
          pstate.z = (result == 0); 
          pstate.c = result < rn->w; 
          pstate.v = (((rn->w < 0) == ((int32_t)imm < 0)) && ((result < 0) != (rn->w < 0))); 
        }
        break;
      case 0b10: // sub
        if (sf)  // 64-bit
        {
          rd->x = rn->x - (int64_t)imm;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = rn->w - (int32_t)imm;
        }
        break;
      case 0b11: // subs
        // printf("%x %x %x\n",rdi,rn->x,imm);
        if (sf)  // 64-bit
        {
          rd->x = rn->x - (int64_t)imm;
          pstate.n = take_bits(rd, 63, 1);
          pstate.z = (rd->ux == 0);
          pstate.c = (imm <= rn->ux);
          pstate.v = (INT64_MIN + (int64_t)imm > rn->x);
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = rn->w - (int32_t)imm;
          pstate.n = take_bits(rd, 31, 1);
          pstate.z = (rd->uw == 0);
          pstate.c = (imm <= rn->uw);
          pstate.v = (INT32_MIN + (int32_t)imm > rn->w);
        }
        break;
      }
    }
    else if ((opr & 0b1000) == 0b0000) // bit-logic
    {
      if (sf) // 64-bit
      {
        if (opr & 0b0001) // negated
        {
          imm = ~bit_shift64(opr, rm->x, operand);
        }
        else // not negated
        {
          imm = bit_shift64(opr, rm->x, operand);
        }
      }
      else // 32-bit
      {
        if (opr & 0b0001) // negated
        {
          imm = ~bit_shift32(opr, rm->w, operand);
        }
        else // not negated
        {
          imm = bit_shift32(opr, rm->w, operand);
        }
      }

      switch (opc)
      {
      case 0b00: // and, bic
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux & imm;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->uw = rn->uw & imm;
        }
        break;
      case 0b01: // orr, orn
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux | imm;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->uw = rn->uw | imm;
        }
        break;
      case 0b10: // eor, eon
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux ^ imm;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->uw = rn->uw ^ imm;
        }
        break;
      case 0b11: // ands, bics
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux & imm;
          pstate.n = take_bits(rd, 63, 1);
          pstate.z = (rd->ux == 0);
          pstate.c = 0;
          pstate.v = 0;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->uw = rn->uw & imm;
          pstate.n = take_bits(rd, 31, 1);
          pstate.z = (rd->uw == 0);
          pstate.c = 0;
          pstate.v = 0;
        }
        break;
      }
    }
  }
}

void ls(instr instr)
{
  seg rti = take_bits(&instr, 0, 5);
  seg xni = take_bits(&instr, 5, 5);
  seg sf = take_bits(&instr, 30, 1);
  seg load = take_bits(&instr, 22, 1);
  seg addr;
  int64_t offset;
  union reg *rt = r + rti;
  union reg *xn = r + xni;

  if (!take_bits(&instr, 31, 1)) // load literal
  {
    seg simm19 = take_bits(&instr, 5, 19);
    offset = (int64_t)((uint64_t)simm19 << 45) >> 45;
    addr = pc.x + offset * 4;
    load = 1;
  }
  else // single data transfer
  {
    if (take_bits(&instr, 24, 1)) // unsigned
    {
      seg imm12 = take_bits(&instr, 10, 12);
      offset = (int64_t)((uint64_t)imm12 << 52) >> 52;
      if (sf) // 64-bit
      {
        addr = xn->ux + offset * 8;
      }
      else // 32-bit
      {
        addr = xn->uw + offset * 4;
      }
    }
    else if (take_bits(&instr, 21, 1)) // register offset
    {
      seg xmi = take_bits(&instr, 16, 5);
      union reg *xm = r + xmi;
      if (sf) // 64-bit
      {
        addr = xn->ux + xm->ux;
      }
      else // 32-bit
      {
        addr = xn->uw + xm->uw;
      }
    }
    else // pre/post-index
    {
      seg simm9 = take_bits(&instr, 12, 9);
      offset = (int64_t)((uint64_t)simm9 << 55) >> 55;
      seg i = take_bits(&instr, 11, 1);
      if (sf) // 64-bit
      {
        addr = xn->ux + i * offset;
        xn->ux += offset;
      }
      else // 32-bit
      {
        addr = xn->uw + i * offset;
        xn->uw += offset;
      }
    }
  }
  if (load) // load
  {
    if (sf) // 64-bit
    {
      // printf("%x\n",addr);
      rt->ux = mem64_load(addr);
    }
    else // 32-bit
    {
      rt->upper[1] = 0;
      rt->uw = mem32_load(addr);
    }
  }
  else // store
  {
    if (sf) // 64-bit
    {
      mem64_store(addr, rt->ux);
    }
    else // 32-bit
    {
      mem32_store(addr, rt->uw);
    }
  }
}

void br(instr instr) 
{
  seg simm26 = take_bits(&instr, 0, 26);
  seg simm19 = take_bits(&instr, 5, 19);
  int64_t offset;
  seg cond;
  bool exec;
  switch (take_bits(&instr, 30, 2))
  {
  case 0b00:                                          // unconditional
    offset = (int64_t)((uint64_t)simm26 << 38) >> 38; // sign ext
    pc.x += offset * 4;
    break;

  case 0b01:                                          // conditional
    offset = (int64_t)((uint64_t)simm19 << 45) >> 45; // sign ext
    cond = take_bits(&instr, 0, 4);
    exec = false;
    switch (cond)
    {
    case 0b0000: // eq
      exec = pstate.z == 1;
      break;
    case 0b0001: // ne
      exec = pstate.z != 1;
      break;
    case 0b1010: // ge
      exec = pstate.n == pstate.v;
      break;
    case 0b1011: // lt
      exec = pstate.n != pstate.v;
      break;
    case 0b1100: // gt
      exec = !pstate.z && pstate.n == pstate.v;
      break;
    case 0b1101: // le
      exec = pstate.z || pstate.n != pstate.v;
      break;
    case 0b1110: // al
      exec = 1;
      break;
    }
    if (exec) // condition fulfilled
    {
      // printf("%x %x\n",pc.x,)
      pc.x += offset * 4;
    }
    else {
      pc.x += 4;
    }
    break;

  case 0b11: // register
  {
    seg xn = take_bits(&instr, 5, 5);
    pc = r[xn];
  }
  }
}

