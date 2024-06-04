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
} r[32], *zr = r + 31, pc, sp;

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

inline uint64_t take_bits(void *from, int begin, int size)
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
  seg opi = take_bits(&instr, 26, 3);
  seg operand = take_bits(&instr, 5, 18);
  seg rdi = take_bits(&instr, 0, 5);
  union reg *rd = r + rdi;
  seg sh = take_bits(&instr, 22, 1);
  seg imm12 = take_bits(&instr, 10, 12) << (sh * 12);
  seg rni = take_bits(&instr, 5, 5);
  union reg *rn = r + rni;
  seg hw = take_bits(&instr, 21, 2);
  seg imm16 = take_bits(&instr, 5, 16) << (hw * 16);

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
        pstate.c = (imm12 > rn->ux);
        pstate.v = (INT64_MIN + (int64_t)imm12 > rn->x);
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->w = rn->w - (int32_t)imm12;
        pstate.n = take_bits(rd, 31, 1);
        pstate.z = (rd->uw == 0);
        pstate.c = (imm12 > rn->uw);
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
        rd->ux = !(uint64_t)imm16;
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->uw = !(uint32_t)imm16;
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
        rd->ux = (uint64_t)imm16 | (rd->ux & !((uint64_t)0xffff << (hw * 16)));
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->uw = (uint32_t)imm16 | (rd->uw & !((uint32_t)0xffff << (hw * 16)));
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
  seg rdi = take_bits(&instr, 0, 4);
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
          rd->upper[1] = 0;
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
          rd->upper[1] = 0;
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
          rd->upper[1] = 0;
          rd->w = rn->w - (int32_t)imm;
        }
        break;
      case 0b11: // subs
        if (sf)  // 64-bit
        {
          rd->x = rn->x - (int64_t)imm;
          pstate.n = take_bits(rd, 63, 1);
          pstate.z = (rd->ux == 0);
          pstate.c = (imm > rn->ux);
          pstate.v = (INT64_MIN + (int64_t)imm > rn->x);
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = rn->w - (int32_t)imm;
          pstate.n = take_bits(rd, 31, 1);
          pstate.z = (rd->uw == 0);
          pstate.c = (imm > rn->uw);
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
  seg rti = take_bits(&instr, 0, 4);
  seg xni = take_bits(&instr, 5, 5);
  seg sf = take_bits(&instr, 30, 1);
  seg load = take_bits(&instr, 22, 1);
  seg addr;
  union reg *rt = r + rti;
  union reg *xn = r + xni;

  if (!take_bits(&instr, 31, 1)) // load literal
  {
    seg simm19 = take_bits(&instr, 5, 19);
    addr = pc.x + simm19 * 4;
    load = 1;
  }
  else // single data transfer
  {
    if (take_bits(&instr, 24, 1)) // unsigned
    {
      seg imm12 = take_bits(&instr, 10, 12);
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
      seg xm = take_bits(&instr, 16, 5);
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
      seg simm9 = take_bits(&instr, 12, 9);
      seg i = take_bits(&instr, 11, 1);
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
      rt->upper[1] = 0;
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

void br(instr instr)
{
  seg simm26 = take_bits(&instr, 0, 26);
  seg simm19 = take_bits(&instr, 5, 19);
  int64_t offset;
  seg cond;
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
    seg xn = take_bits(&instr, 5, 5);
    pc = r[xn];
  }
  }
}

int main(int argc, char **argv)
{
  char *input_filename, *output_filename;
  if (argc == 1) // terminal output
  {
    input_filename = argv[0];
  }
  else if (argc == 2) // file output
  {
    input_filename = argv[0];
    output_filename = argv[1];
  }
  else
  {
    printf("Invalid number of arguments!\nExpected: emulate input_file [output_file]\n");
    return EXIT_SUCCESS;
  }

  FILE *input_file = fopen(input_filename, "rb");
  instr *buffer;
  seg op0;

  if (input_file == NULL)
  {
    printf("Invalid input file!\n");
  }

  while (!feof(input_file))
  {
    fread(buffer, sizeof(buffer), 1, input_file);
    op0 = take_bits(buffer, 25, 4);
    if ((op0 & 0b1110) == 0b1000) // data processing (immediate)
    {
      dpi(*buffer);
    }
    else if ((op0 & 0b0111) == 0b0101) // data processing (register)
    {
      dpr(*buffer);
    }
    else if ((op0 & 0b0101) == 0b0100) // loads and stores
    {
      ls(*buffer);
    }
    else if ((op0 & 0b1110) == 0b1010) // branches
    {
      br(*buffer);
    }
    else if (op0 == 0x8a000000) // halting
    {
      break;
    }
  }
  fclose(input_file);

  if (output_filename)
  {
    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL)
    {
      printf("Invalid output file!");
      return EXIT_SUCCESS;
    }
    fprintf(output_file, "Registers:\n");
    for (int i = 0; i < 31; i++)
    {
      fprintf(output_file, "X%02d    = %016llx\n", i, r[i].ux);
    }
    fprintf(output_file, "PC     = %016llx\n", pc.ux);
    fprintf(output_file, "PSTATE : %c%c%c%c\n", pstate.n ? 'N' : '-', pstate.z ? 'Z' : '-', pstate.c ? 'C' : '-', pstate.v ? 'V' : '-');
    fprintf(output_file, "Non-zero memory:\n");
    for (uint64_t i = 0; i < MEM_SIZE; i += 8)
    {
      fprintf(output_file, "%#010llx: %#010llx\n", i, mem64_load(i));
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
    for (int64_t i = 0; i < MEM_SIZE; i += 8)
    {
      printf("%#010llx: %#010llx\n", i, mem64_load(i));
    }
  }

  return EXIT_SUCCESS;
}
