
#include "emulate.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define LINE_BY_LINE_PRINT_DEBUG // to print the resulting state after each instruction (warning: affects performance)

uint8_t mem[MEM_SIZE]; // memory

reg_t r[32], *zr = r + 31, pc; // registers

struct
{
  unsigned int n : 1, z : 1, c : 1, v : 1;
} pstate = {0, 1, 0, 0}; // p-states

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

// take a segment of a binary starting from begin of size size
inline uint64_t take_bits(void *from, int begin, int size)
{
  uint64_t i = *(uint64_t *)from;
  i >>= begin;
  i &= (1 << size) - 1;
  return i;
}

// shift 32-bit operand by shift_amount under the mode described in opr
uint32_t bit_shift32(seg_t opr, seg_t operand, int shift_amount)
{
  switch (opr & 0b0110)
  {
  case 0b0000: // lsl
    return operand << shift_amount;
    break;
  case 0b0010: // lsr
    return operand >> shift_amount;
    break;
  case 0b0100: // asr
    return (operand >> shift_amount) | (take_bits(&operand, 31, 1) * (UINT32_MAX << (32 - shift_amount)));
    break;
  case 0b0110: // ror
    return (operand >> shift_amount) | (operand << (32 - shift_amount));
    break;
  }
  return 0;
}

// shift 64-bit operand by shift_amount under the mode described in opr
uint64_t bit_shift64(seg_t opr, uint64_t operand, int shift_amount)
{
  switch (opr & 0b0110)
  {
  case 0b0000: // lsl
    return operand << shift_amount;
    break;
  case 0b0010: // lsr
    return operand >> shift_amount;
    break;
  case 0b0100: // asr
    return (operand >> shift_amount) | (take_bits(&operand, 63, 1) * (UINT64_MAX << (64 - shift_amount)));
    break;
  case 0b0110: // ror
    return (operand >> shift_amount) | (operand << (64 - shift_amount));
    break;
  }
  return 0;
}

// data processing (immediate)
void dpi(instr_t instr)
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
        rd->x = rn->x + ((int64_t)imm12);
      }
      else // 32-bit
      {
        rd->UPPER = 0;
        rd->w = rn->w + ((int32_t)imm12);
      }
      break;
    case 0b01: // adds
      if (sf)  // 64-bit
      {
        rd->ux = rn->ux + ((uint64_t)imm12);
        pstate.n = take_bits(rd, 63, 1);
        pstate.z = (rd->ux == 0);
        pstate.c = (UINT64_MAX - (imm12) < rn->ux);
        pstate.v = (INT64_MAX - ((int64_t)imm12) < rn->x);
      }
      else // 32-bit
      {
        rd->UPPER = 0;
        rd->uw = rn->uw + ((uint32_t)imm12);
        pstate.n = take_bits(rd, 31, 1);
        pstate.z = (rd->uw == 0);
        pstate.c = (UINT32_MAX - (imm12) < rn->uw);
        pstate.v = (INT32_MAX - ((int32_t)imm12) < rn->w);
      }
      break;
    case 0b10: // sub
      if (sf)  // 64-bit
      {
        rd->x = rn->x - ((int64_t)imm12);
      }
      else // 32-bit
      {
        rd->UPPER = 0;
        rd->w = rn->w - ((int32_t)imm12);
      }
      break;
    case 0b11: // subs
      if (sf)  // 64-bit
      {
        rd->x = rn->x - ((int64_t)imm12);
        pstate.n = take_bits(rd, 63, 1);
        pstate.z = (rd->ux == 0);
        pstate.c = !((imm12) > rn->ux);
        pstate.v = (INT64_MIN + ((int64_t)imm12) > rn->x);
      }
      else // 32-bit
      {
        rd->UPPER = 0;
        rd->w = rn->w - ((int32_t)imm12);
        pstate.n = take_bits(rd, 31, 1);
        pstate.z = (rd->uw == 0);
        pstate.c = !((imm12) > rn->uw);
        pstate.v = (INT32_MIN + ((int32_t)imm12) > rn->w);
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
        rd->ux = ~((uint64_t)imm16);
      }
      else // 32-bit
      {
        rd->UPPER = 0;
        rd->uw = ~((uint32_t)imm16);
      }
      break;
    case 0b10: // movz
      if (sf)  // 64-bit
      {
        rd->ux = ((uint64_t)imm16);
      }
      else // 32-bit
      {
        rd->UPPER = 0;
        rd->uw = ((uint32_t)imm16);
      }
      break;
    case 0b11: // movk
      if (sf)  // 64-bit
      {
        rd->ux = ((uint64_t)imm16) | (rd->ux & ~((uint64_t)0xffff << (hw * 16)));
      }
      else // 32-bit
      {
        rd->UPPER = 0;
        rd->uw = ((uint32_t)imm16)  | (rd->uw & ~((uint32_t)0xffff << (hw * 16)));
      }
      break;
    }
    break;
  }
  zr->uw = 0; // clear zero counter
  pc.ux += 4; // update program counter
}

// data processing (register)
void dpr(instr_t instr)
{
  seg_t sf = take_bits(&instr, 31, 1);
  seg_t opc = take_bits(&instr, 29, 2);
  seg_t m = take_bits(&instr, 28, 1);
  seg_t opr = take_bits(&instr, 21, 4);
  seg_t rmi = take_bits(&instr, 16, 5);
  uint64_t operand = take_bits(&instr, 10, 6);
  seg_t rni = take_bits(&instr, 5, 5);
  seg_t rdi = take_bits(&instr, 0, 5);
  seg_t rai = take_bits(&instr, 10, 5);
  reg_t *rm = r + rmi;
  reg_t *rn = r + rni;
  reg_t *rd = r + rdi;
  reg_t *ra = r + rai;

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
        operand = bit_shift64(opr, rm->x, operand);
      }
      else // 32-bit
      {
        operand = bit_shift32(opr, rm->uw, operand);
      }
      switch (opc)
      {
      case 0b00: // add
        if (sf)  // 64-bit
        {
          rd->x = rn->x + (int64_t)operand;
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->w = rn->w + (int32_t)operand;
        }
        break;
      case 0b01: // adds
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux + (uint64_t)operand;
          pstate.n = (rd->ux >> 63) & 0b1;
          pstate.z = (rd->ux == 0);
          pstate.c = (UINT64_MAX - (uint32_t)operand < rn->ux);
          pstate.v = ((rn->x < 0) && ((int64_t)operand < 0) && (rd->x > 0)) || ((rn->x > 0) && ((int64_t)operand > 0) && (rd->x < 0));
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->uw = rn->uw + (uint32_t)operand;
          pstate.n = (rd->uw >> 31) & 0b1;
          pstate.z = (rd->uw == 0);
          pstate.c = (UINT32_MAX - (uint32_t)operand < rn->uw);
          pstate.v = ((rn->w < 0) && ((int32_t)operand < 0) && (rd->w > 0)) || ((rn->w > 0) && ((int32_t)operand > 0) && (rd->w < 0));
        }
        break;
      case 0b10: // sub
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux - (uint64_t)operand;
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->w = rn->w - (uint32_t)operand;
        }
        break;
      case 0b11: // subs
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux - (uint64_t)operand;
          pstate.n = take_bits(rd, 63, 1);
          pstate.z = (rd->ux == 0);
          pstate.c = !((uint64_t)operand > rn->ux);
          pstate.v = ((rn->x < 0) && ((int64_t)operand > 0) && (rd->x > 0)) || ((rn->x > 0) && ((int64_t)operand < 0) && (rd->x < 0));
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->uw = rn->uw - (uint32_t)operand;
          pstate.n = take_bits(rd, 31, 1);
          pstate.z = (rd->uw == 0);
          pstate.c = !((uint32_t)operand > rn->uw);
          pstate.v = ((rn->w < 0) && ((int32_t)operand > 0) && (rd->w > 0)) || ((rn->w > 0) && ((int32_t)operand < 0) && (rd->w < 0));
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
          operand = ~bit_shift64(opr, rm->ux, operand);
        }
        else // not negated
        {
          operand = bit_shift64(opr, rm->ux, operand);
        }
      }
      else // 32-bit
      {
        if (opr & 0b0001) // negated
        {
          operand = ~bit_shift32(opr, rm->uw, operand);
        }
        else // not negated
        {
          operand = bit_shift32(opr, rm->uw, operand);
        }
      }

      switch (opc)
      {
      case 0b00: // and, bic
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux & operand;
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->uw = rn->uw & operand;
        }
        break;
      case 0b01: // orr, orn
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux | operand;
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->uw = rn->uw | operand;
        }
        break;
      case 0b10: // eor, eon
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux ^ operand;
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->uw = rn->uw ^ operand;
        }
        break;
      case 0b11: // ands, bics
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux & operand;
          pstate.n = take_bits(rd, 63, 1);
          pstate.z = (rd->ux == 0);
          pstate.c = 0;
          pstate.v = 0;
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->uw = rn->uw & operand;
          pstate.n = take_bits(rd, 31, 1);
          pstate.z = (rd->uw == 0);
          pstate.c = 0;
          pstate.v = 0;
        }
        break;
      }
    }
  }
  zr->uw = 0; // clear zero counter
  pc.ux += 4; // update program counter
}

// loads and stores
void ls(instr_t instr)
{
  seg_t rti = take_bits(&instr, 0, 5);
  seg_t xni = take_bits(&instr, 5, 5);
  seg_t sf = take_bits(&instr, 30, 1);
  seg_t load = take_bits(&instr, 22, 1);
  seg_t addr;
  int64_t offset;
  reg_t *rt = r + rti;
  reg_t *xn = r + xni;
  seg_t simm19 = take_bits(&instr, 5, 19);
  seg_t imm12 = take_bits(&instr, 10, 12);
  seg_t xmi = take_bits(&instr, 16, 5);
  reg_t *xm = r + xmi;
  seg_t simm9 = take_bits(&instr, 12, 9);
  seg_t i = take_bits(&instr, 11, 1);

  if (!take_bits(&instr, 31, 1)) // load literal
  {
    offset = (int64_t)((uint64_t)simm19 << 45) >> 45;
    addr = pc.x + offset * 4;
    load = 1;
  }
  else // single data transfer
  {
    if (take_bits(&instr, 24, 1)) // unsigned
    {
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
      offset = (int64_t)((uint64_t)simm9 << 55) >> 55;
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
      rt->ux = mem64_load(addr);
    }
    else  // 32-bit
    {
      rt->UPPER = 0;
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
  zr->uw = 0; // clear zero counter
  pc.ux += 4; // update program counter
}

// branches
void br(instr_t instr)
{
  seg_t simm26 = take_bits(&instr, 0, 26);
  seg_t simm19 = take_bits(&instr, 5, 19);
  int64_t offset;
  seg_t cond;
  int8_t exec;
  switch (take_bits(&instr, 30, 2))
  {
  case 0b00: // unconditional
    offset = (int64_t)((uint64_t)simm26 << 38) >> 38; // sign ext
    pc.x += offset * 4; // jump to specified offset
    break;

  case 0b01: // conditional
    offset = (int64_t)((uint64_t)simm19 << 45) >> 45; // sign ext
    cond = take_bits(&instr, 0, 4);
    exec = 0;
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
      pc.ux += offset * 4; // jump to specified offset
    }
    else // condition failed
    {
      pc.ux += 4; // update program counter
    }
    break;

  case 0b11: // register
  {
    seg_t xn = take_bits(&instr, 5, 5);
    pc.uw = (r + xn)->uw; // jump to address stored in register
  }
  break;
  }
  // program counter not updated here as all branches leads to their own update mode
}

// print all registers and non-zero memories
void print_state()
{
  printf("Registers:\n");
  for (int i = 0; i < 31; i++)
  {
    printf("X%02d    = %016llx\n", i, r[i].ux); // e.g. "X05    = 00000000ffffffc4\n"
  }
  printf("PC     = %016llx\n", pc.ux);          // e.g. "PC     = 00000000ffffffc4\n"
  printf("PSTATE : %c%c%c%c\n", pstate.n ? 'N' : '-', pstate.z ? 'Z' : '-', pstate.c ? 'C' : '-', pstate.v ? 'V' : '-');
                                                // e.g. "PSTATE : Z-C-\n"
  printf("Non-zero memory:\n");
  for (int64_t i = 0; i < MEM_SIZE; i += 4)
  {
    uint64_t m = mem32_load(i);
    if (m) // non-zero memory
    {
    printf("%#010llx: %08llx\n", i, m);         // e.g. "0x00000004: 00a30b92\n"
    }
  }
}

// print all registers and non-zero memories to a file
void fprint_state(FILE *out)
{
  fprintf(out, "Registers:\n");
    for (int i = 0; i < 31; i++)
    {
      fprintf(out, "X%02d    = %016llx\n", i, r[i].ux); // e.g. "X05    = 00000000ffffffc4\n"
    }
    fprintf(out, "PC     = %016llx\n", pc.ux);          // e.g. "PC     = 00000000ffffffc4\n"
    fprintf(out, "PSTATE : %c%c%c%c\n", pstate.n ? 'N' : '-', pstate.z ? 'Z' : '-', pstate.c ? 'C' : '-', pstate.v ? 'V' : '-');
                                                        // e.g. "PSTATE : Z-C-\n"
    fprintf(out, "Non-zero memory:\n");
    for (uint64_t i = 0; i < MEM_SIZE; i += 4)
    {
      uint64_t m = mem32_load(i);
      if (!m)
      {
        continue;
      }
      fprintf(out, "%#010llx: %08llx\n", i, m);         // e.g. "0x00000004: 00a30b92\n"
    }
}

int main(int argc, char **argv)
{
  if (argc == 2) // terminal output
  {
    printf("Input file: %s\n", argv[1]);
  }
  else if (argc == 3) // file output
  {
    printf("Input file: %s\n", argv[1]);
    printf("Output file: %s\n", argv[2]);
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
    instr_t instr = mem32_load(pc.ux);
    op0 = take_bits(&instr, 25, 4);
    if (instr == 0x8a000000) // halting
    {
      break;
    }
    if ((op0 & 0b1110) == 0b1000) // data processing (immediate)
    {
      dpi(instr);
    }
    else if ((op0 & 0b0111) == 0b0101) // data processing (register)
    {
      dpr(instr);
    }
    else if ((op0 & 0b0101) == 0b0100) // loads and stores
    {
      ls(instr);
    }
    else if ((op0 & 0b1110) == 0b1010) // branches
    {
      br(instr);
    }
    else // unknown command
    {
      printf("Invalid command at location %#08x: %#08x. Program aborted.\n", pc.uw, instr);
      break;
    }
    #ifdef LINE_BY_LINE_PRINT_DEBUG
    print_state();
    #endif
  }

  if (argc == 3) // file output
  {
    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL) // invalid output file
    {
      printf("Invalid output file!");
      return EXIT_FAILURE;
    }
    fprint_state(output_file);
    fclose(output_file);
  }
  else // terminal output
  {
    print_state();
  }

  return EXIT_SUCCESS;
}
