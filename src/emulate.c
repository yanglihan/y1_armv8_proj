#include <stdlib.h>
#include <stdbool.h>

typedef uint32_t instr;
typedef uint32_t seg; // using seg for a segment of instructions
union reg
{
  int64_t x;
  int32_t w;
  uint64_t ux;
  uint32_t uw;
} r[32], *zr = r + 31, pc, sp; // registers

struct
{
  bool n, z, c, v;
} pstate = {0, 1, 0, 0};

int main(int argc, char **argv)
{

  return EXIT_SUCCESS;
}

uint64_t takebits(void *from, int begin, int size)
{
  uint64_t i = *(uint64_t *)from;
  i >>= begin;
  i &= (1 << size) - 1;
  return i;
}

void dpi(instr instr)
{
  seg sf = takebits(&instr, 31, 1);
  seg opc = takebits(&instr, 29, 2);
  seg opi = takebits(&instr, 26, 3);
  seg opr = takebits(&instr, 5, 18);
  seg rdi = takebits(&instr, 0, 5);
  union reg *rd = r + rdi;
  seg sh = takebits(&instr, 22, 1);
  seg imm12 = takebits(&opr, 10, 12) << (sh * 12);
  seg rni = takebits(&opr, 5, 5);
  union reg *rn = r + rni;
  seg hw = takebits(&instr, 21, 2);
  seg imm16 = takebits(&instr, 5, 16) << (hw * 16);

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
        rd->w = rn->w + (int32_t)imm12;
      }
      break;
    case 0b01: // adds
      if (sh)  // 64-bit
      {
        rd->x = rn->x + (int64_t)imm12;
        pstate.n = takebits(rd, 63, 1);
        pstate.z = (rd->x == 0);
        pstate.c = (UINT64_MAX - imm12 > rn->x);
        pstate.v = (INT64_MAX - (int64_t)imm12 > rn->x);
      }
      else // 32-bit
      {
        rd->w = rn->w + (int32_t)imm12;
        pstate.n = takebits(rd, 31, 1);
        pstate.z = (rd->w == 0);
        pstate.c = (UINT32_MAX - imm12 > rn->w);
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
        rd->w = rn->w - (int32_t)imm12;
      }
      break;
    case 0b11: // subs
      if (sh)  // 64-bit
      {
        rd->x = rn->x - (int64_t)imm12;
        pstate.n = takebits(rd, 63, 1);
        pstate.z = (rd->x == 0);
        pstate.c = (imm12 > rn->x);
        pstate.v = (INT64_MIN + (int64_t)imm12 > rn->x);
      }
      else // 32-bit
      {
        rd->w = rn->w - (int32_t)imm12;
        pstate.n = takebits(rd, 31, 1);
        pstate.z = (rd->w == 0);
        pstate.c = (imm12 > rn->w);
        pstate.v = (INT32_MIN + (int32_t)imm12 > rn->w);
      }
      break;
    default:
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
        rd->ux = !(uint32_t)imm16; // ux used purposefully to set upper half to zero
      }
      break;
    case 0b10: // movz
      if (sf)  // 64-bit
      {
        rd->ux = (uint64_t)imm16;
      }
      else // 32-bit
      {
        rd->ux = (uint32_t)imm16; // ux used purposefully to set upper half to zero
      }
      break;
    case 0b11: // movk
      if (sf)  // 64-bit
      {
        rd->ux = (uint64_t)imm16 | (rd->ux & !((uint64_t)0xffff << (hw * 16)));
      }
      else // 32-bit
      {
        rd->ux = (uint32_t)imm16 | (rd->uw & !((uint32_t)0xffff << (hw * 16)));  // ux used purposefully to set upper half to zero
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}