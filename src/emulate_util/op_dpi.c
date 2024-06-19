#include "op_dpi.h"

#include "bitwise.h"
#include "state.h"
#include "../common/consts.h"

// data processing (immediate)
void dpi(instr_t instr)
{
  seg_t sf = bitfield(&instr, 31, 1);
  seg_t opc = bitfield(&instr, 29, 2);
  seg_t opi = bitfield(&instr, 23, 3);
  seg_t operand = bitfield(&instr, 5, 18);
  seg_t rdi = bitfield(&instr, 0, 5);
  reg_t *rd = r + rdi;
  seg_t sh = bitfield(&instr, 22, 1);
  seg_t imm12 = bitfield(&instr, 10, 12) << (sh * 12);
  seg_t rni = bitfield(&instr, 5, 5);
  reg_t *rn = r + rni;
  seg_t hw = bitfield(&instr, 21, 2);
  seg_t imm16 = bitfield(&instr, 5, 16) << (hw * 16);

  switch (opi)
  {
  case DPI_OPI_AR: // arithmatic operation
    switch (opc)
    {
    case ARITH_OPC_ADD: // add
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
    case ARITH_OPC_ADDS: // adds
      if (sf)  // 64-bit
      {
        rd->ux = rn->ux + ((uint64_t)imm12);
        pstate.n = bitfield(rd, 63, 1);
        pstate.z = (rd->ux == 0);
        pstate.c = (UINT64_MAX - (imm12) < rn->ux);
        pstate.v = (INT64_MAX - ((int64_t)imm12) < rn->x);
      }
      else // 32-bit
      {
        rd->UPPER = 0;
        rd->uw = rn->uw + ((uint32_t)imm12);
        pstate.n = bitfield(rd, 31, 1);
        pstate.z = (rd->uw == 0);
        pstate.c = (UINT32_MAX - (imm12) < rn->uw);
        pstate.v = (INT32_MAX - ((int32_t)imm12) < rn->w);
      }
      break;
    case ARITH_OPC_SUB: // sub
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
    case ARITH_OPC_SUBS: // subs
      if (sf)  // 64-bit
      {
        rd->x = rn->x - ((int64_t)imm12);
        pstate.n = bitfield(rd, 63, 1);
        pstate.z = (rd->ux == 0);
        pstate.c = !((imm12) > rn->ux);
        pstate.v = (INT64_MIN + ((int64_t)imm12) > rn->x);
      }
      else // 32-bit
      {
        rd->UPPER = 0;
        rd->w = rn->w - ((int32_t)imm12);
        pstate.n = bitfield(rd, 31, 1);
        pstate.z = (rd->uw == 0);
        pstate.c = !((imm12) > rn->uw);
        pstate.v = (INT32_MIN + ((int32_t)imm12) > rn->w);
      }
      break;
    }
    break;
  case DPI_OPI_WM: // wide move operation
    switch (opc)
    {
    case DPI_OPC_MOVN: // movn
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
    case DPI_OPC_MOVZ: // movz
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
    case DPI_OPC_MOVK: // movk
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
  RESET_ZR; // clear zero counter
  PC_INC; // update program counter
}
