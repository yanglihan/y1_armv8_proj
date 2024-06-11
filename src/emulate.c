
#include "emulate.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t mem[MEM_SIZE]; // memory

reg_t r[32], *zr = r + 31, pc; // registers

struct
{
  int n : 1, z : 1, c : 1, v : 1;
} pstate = {0, 1, 0, 0}; // p-states

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

uint64_t take_bits(void *from, int begin, int size)
{
  uint64_t i = *(uint64_t *)from;
  i >>= begin;
  i &= (1 << size) - 1;
  return i;
}

uint32_t bit_shift32(seg_t opr, seg_t oprand, int shift_amount)
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

uint64_t bit_shift64(seg_t opr, uint64_t oprand, int shift_amount)
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

void dpi(instr_t instr) // data processing (immediate)
{
  seg_t sf = take_bits(&instr, 31, 1);
  seg_t opc = take_bits(&instr, 29, 2);
  seg_t opi = take_bits(&instr, 23, 3);
  seg_t operand = take_bits(&instr, 5, 18);
  seg_t rdi = take_bits(&instr, 0, 5);
  reg_t *rd = r + rdi;
  seg_t sh = take_bits(&instr, 22, 1);
  seg_t imm12 = take_bits(&instr, 10, 12) << (sh * 12);
  seg_t rni = take_bits(&instr, 5, 5);
  reg_t *rn = r + rni;
  seg_t hw = take_bits(&instr, 21, 2);
  seg_t imm16 = take_bits(&instr, 5, 16) << (hw * 16);

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
        rd->UPPER = 0;
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
        rd->UPPER = 0;
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
        rd->UPPER = 0;
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
        rd->UPPER = 0;
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
        rd->UPPER = 0;
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
        rd->UPPER = 0;
        rd->uw = (uint32_t)imm16;
      }
      break;
    case 0b11: // movk
      if (sf)  // 64-bit
      {
        rd->ux = (uint64_t)imm16 | (rd->ux & ~((uint64_t)0xffff << (hw * 16)));
      }
      else // 32-bit
      {
        rd->UPPER = 0;
        rd->uw = (uint32_t)imm16 | (rd->uw & ~((uint32_t)0xffff << (hw * 16)));
      }
      break;
    }
    break;
  }
  zr->uw = 0;
}

void dpr(instr_t instr) // data processing (register)
{
  seg_t sf = take_bits(&instr, 31, 1);
  seg_t opc = take_bits(&instr, 29, 2);
  seg_t m = take_bits(&instr, 28, 1);
  seg_t opr = take_bits(&instr, 21, 4);
  seg_t rmi = take_bits(&instr, 16, 5);
  seg_t operand = take_bits(&instr, 10, 6);
  seg_t rni = take_bits(&instr, 5, 5);
  seg_t rdi = take_bits(&instr, 0, 4);
  seg_t rai = take_bits(&instr, 10, 5);
  reg_t *rm = r + rmi;
  reg_t *rn = r + rni;
  reg_t *rd = r + rdi;
  reg_t *ra = r + rai;
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
          rd->UPPER = 0;
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
          rd->UPPER = 0;
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
        imm = bit_shift32(opr, rm->x, operand);
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
          rd->UPPER = 0;
          rd->w = rn->w + (int32_t)imm;
        }
        break;
      case 0b01: // adds
        if (sf)  // 64-bit
        {
          rd->x = rn->x + (int64_t)imm;
          pstate.n = take_bits(rd, 63, 1);
          pstate.z = (rd->ux == 0);
          pstate.c = (UINT64_MAX - imm > rn->ux);
          pstate.v = (INT64_MAX - (int64_t)imm > rn->x);
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->w = rn->w + (int32_t)imm;
          pstate.n = take_bits(rd, 31, 1);
          pstate.z = (rd->uw == 0);
          pstate.c = (UINT32_MAX - imm > rn->uw);
          pstate.v = (INT32_MAX - (int32_t)imm > rn->w);
        }
        break;
      case 0b10: // sub
        if (sf)  // 64-bit
        {
          rd->x = rn->x - (int64_t)imm;
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->w = rn->w - (int32_t)imm;
        }
        break;
      case 0b11: // subs
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
          rd->UPPER = 0;
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
          rd->UPPER = 0;
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
          rd->UPPER = 0;
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
          rd->UPPER = 0;
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
          rd->UPPER = 0;
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

void ls(instr_t instr) // loads and stores
{
  seg_t rti = take_bits(&instr, 0, 4);
  seg_t xni = take_bits(&instr, 5, 5);
  seg_t sf = take_bits(&instr, 30, 1);
  seg_t load = take_bits(&instr, 22, 1);
  seg_t addr;
  reg_t *rt = r + rti;
  reg_t *xn = r + xni;

  if (!take_bits(&instr, 31, 1)) // load literal
  {
    seg_t simm19 = take_bits(&instr, 5, 19);
    addr = pc.x + simm19 * 4;
    load = 1;
  }
  else // single data transfer
  {
    if (take_bits(&instr, 24, 1)) // unsigned
    {
      seg_t imm12 = take_bits(&instr, 10, 12);
      if (sf) // 64-bit
      {
        addr = xn->ux + imm12 * 8;
      }
      else // 32-bit
      {
        addr = xn->uw + imm12 * 4;
      }
    }
    else if (take_bits(&instr, 21, 1)) // register offset
    {
      seg_t xm = take_bits(&instr, 16, 5);
      if (sf) // 64-bit
      {
        addr = xn->ux + xm;
      }
      else // 32-bit
      {
        addr = xn->uw + xm;
      }
    }
    else // pre/post-index
    {
      seg_t simm9 = take_bits(&instr, 12, 9);
      seg_t i = take_bits(&instr, 11, 1);
      if (sf) // 64-bit
      {
        addr = xn->ux + i * simm9;
        xn->ux += simm9;
      }
      else // 32-bit
      {
        addr = xn->uw + i * simm9;
        xn->uw += simm9;
      }
    }
  }
  if (load) // load
  {
    if (sf) // 64-bit
    {
      rt->ux = mem64_load(addr);
    }
    else // 32-bit
    {
      rt->UPPER = 0;
      rt->uw = mem32_load(addr);
    }
  }
  else // store
  {
    if (sf) // 64-bit
    {
      mem64_store(addr, xn->ux);
    }
    else // 32-bit
    {
      mem32_store(addr, xn->uw);
    }
  }
}

void br(instr_t instr) // branches
{
  seg_t simm26 = take_bits(&instr, 0, 26);
  seg_t simm19 = take_bits(&instr, 5, 19);
  int64_t offset;
  seg_t cond;
  int8_t exec;
  switch (take_bits(&instr, 30, 2))
  {
  case 0b00:                                          // unconditional
    offset = (int64_t)((uint64_t)simm26 << 38) >> 38; // sign ext
    pc.x += offset * 4;
    break;

  case 0b01:                                          // conditional
    offset = (int64_t)((uint64_t)simm19 << 45) >> 45; // sign ext
    cond = take_bits(&instr, 0, 4);
    exec = 0;
    switch (cond)
    {
    case 0b0000: // eq
      exec = pstate.z;
      break;
    case 0b0001: // ne
      exec = !pstate.z;
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
      pc.x += offset;
    }
    break;

  case 0b11: // register
  {
    seg_t xn = take_bits(&instr, 5, 5);
    pc = r[xn];
  }
  }
}

int main(int argc, char **argv)
{
  printf("argc: %d\n", argc);
  if (argc == 2) // terminal output
  {
    // input_filename = malloc(strlen(argv[1]));
    printf("Input file: %s\n", argv[1]);
    // strcpy(input_filename, argv[1]);
  }
  else if (argc == 3) // file output
  {
    // input_filename = malloc(strlen(argv[1]));
    // output_filename = malloc(strlen(argv[2]));
    printf("Input file: %s\n", argv[1]);
    printf("Output file: %s\n", argv[2]);
    // strcpy(input_filename, argv[1]);
    // strcpy(output_filename, argv[2]);
  }
  else
  {
    printf("Invalid number of arguments!\nExpected: emulate input_file [output_file]\n");
    return EXIT_FAILURE;
  }

  FILE *input_file = fopen(argv[1], "rb");

  if (input_file == NULL) // invalid source file
  {
    printf("Invalid input file!\n");
    return EXIT_FAILURE;
  }

  fread(mem, MEM_SIZE, 1, input_file);
  fclose(input_file);

  while (1)
  {
    seg_t op0;
    printf("PC: %08x\n", pc.uw);
    instr_t instr = mem32_load(pc.ux);
    op0 = take_bits(&instr, 25, 4);
    if (instr == 0x8a000000) // halting
    {
      printf("-- HLT: %08x\n", instr);
      break;
    }
    if ((op0 & 0b1110) == 0b1000) // data processing (immediate)
    {
      printf("-- DPI: %08x\n", instr);
      dpi(instr);
    }
    else if ((op0 & 0b0111) == 0b0101) // data processing (register)
    {
      printf("-- DPR: %08x\n", instr);
      dpr(instr);
    }
    else if ((op0 & 0b0101) == 0b0100) // loads and stores
    {
      printf("-- LS : %08x\n", instr);
      ls(instr);
    }
    else if ((op0 & 0b1110) == 0b1010) // branches
    {
      printf("-- BR : %08x\n", instr);
      br(instr);
    }
    else // unknown command
    {
      printf("-- UNK: %08x\n", instr);
      break;
    }
    pc.ux += 4;
  }

  if (argc == 3)
  {
    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL)
    {
      printf("Invalid output file!");
      return EXIT_FAILURE;
    }
    fprintf(output_file, "Registers:\n");
    for (int i = 0; i < 31; i++)
    {
      fprintf(output_file, "X%02d    = %016llx\n", i, r[i].ux);
    }
    fprintf(output_file, "PC     = %016llx\n", pc.ux);
    fprintf(output_file, "PSTATE : %c%c%c%c\n", pstate.n ? 'N' : '-', pstate.z ? 'Z' : '-', pstate.c ? 'C' : '-', pstate.v ? 'V' : '-');
    fprintf(output_file, "Non-zero memory:\n");
    for (uint64_t i = 0; i < MEM_SIZE; i += 4)
    {
      uint64_t m = mem32_load(i);
      if (!m)
      {
        continue;
      }
      fprintf(output_file, "%#010llx: %#010llx\n", i, m);
    }
    fclose(output_file);
  }
  else
  {
    printf("Registers:\n");
    for (int i = 0; i < 31; i++)
    {
      printf("X%02d    = %016llx\n", i, r[i].ux);
    }
    printf("PC     = %016llx\n", pc.ux);
    printf("PSTATE : %c%c%c%c\n", pstate.n ? 'N' : '-', pstate.z ? 'Z' : '-', pstate.c ? 'C' : '-', pstate.v ? 'V' : '-');
    printf("Non-zero memory:\n");
    for (int64_t i = 0; i < MEM_SIZE; i += 4)
    {
      uint64_t m = mem32_load(i);
      if (!m)
      {
        continue;
      }
      printf("%#010llx: %#010llx\n", i, m);
    }
  }

  return EXIT_SUCCESS;
}
