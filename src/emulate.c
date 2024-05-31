#include <stdbool.h>
#include <stdlib.h>

typedef uint32_t instr;
typedef uint32_t seg; // using seg for a segment of instructions
union reg
{
  int64_t x;
  int32_t w;
  uint64_t ux;
  uint32_t uw;
  uint32_t upper[2];
} r[32], *zr = r + 31, pc, sp; // registers

struct
{
  bool n, z, c, v;
} pstate = {0, 1, 0, 0};

int main(int argc, char **argv)
{

  return EXIT_SUCCESS;
}

uint64_t take_bits(void *from, int begin, int size)
{
  uint64_t i = *(uint64_t *)from;
  i >>= begin;
  i &= (1 << size) - 1;
  return i;
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
    if (opr & 0b1001 == 0b1000) // arithmatic
    {
      if (sf) // 64-bit
      {
        imm = bit_shift64(opr, rm, operand);
      }
      else // 32-bit
      {
        imm = bit_shift32(opr, rm, operand);
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
    else if (opr & 0b1000 == 0b0000) // bit-logic
    {
      if (sf) // 64-bit
      {
        if (opr & 0b0001) // negated
        {
          imm = ~bit_shift64(opr, rm, operand);
        }
        else // not negated
        {
          imm = bit_shift64(opr, rm, operand);
        }
      }
      else // 32-bit
      {
        if (opr & 0b0001) // negated
        {
          imm = ~bit_shift32(opr, rm, operand);
        }
        else // not negated
        {
          imm = bit_shift32(opr, rm, operand);
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
    return (oprand >> shift_amount) | (take_bits(oprand, 63, 1) * (UINT64_MAX << (64 - shift_amount)));
    break;
  case 0b0110: // ror
    return (oprand >> shift_amount) | (oprand << (64 - shift_amount));
    break;
  }
  return 0;
}
