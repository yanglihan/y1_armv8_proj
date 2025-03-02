#include "op_dpr.h"

#include "bitwise.h"
#include "state.h"
#include "../common/consts.h"

// data processing (register)
void dpr(instr_t instr)
{
  seg_t sf = bitfield(&instr, 31, 1);
  seg_t opc = bitfield(&instr, 29, 2);
  seg_t m = bitfield(&instr, 28, 1);
  seg_t opr = bitfield(&instr, 21, 4);
  seg_t shift = bitfield(&instr, 22, 2);
  seg_t rmi = bitfield(&instr, 16, 5);
  uint64_t operand = bitfield(&instr, 10, 6);
  seg_t rni = bitfield(&instr, 5, 5);
  seg_t rdi = bitfield(&instr, 0, 5);
  seg_t rai = bitfield(&instr, 10, 5);
  reg_t* rm = r + rmi;
  reg_t* rn = r + rni;
  reg_t* rd = r + rdi;
  reg_t* ra = r + rai;

  if (m) // multiply operations
  {
    if (C_DPR_OPR_MUL(opr)) // multiply
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
    if (C_DPR_OPR_AR(opr)) // arithmatic
    {
      if (sf) // 64-bit
      {
        operand = bitshift64(shift, rm->x, operand);
      }
      else // 32-bit
      {
        operand = bitshift32(shift, rm->uw, operand);
      }
      switch (opc)
      {
      case ARITH_OPC_ADD: // add
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
      case ARITH_OPC_ADDS: // adds
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
      case ARITH_OPC_SUB: // sub
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
      case ARITH_OPC_SUBS: // subs
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux - (uint64_t)operand;
          pstate.n = bitfield(rd, 63, 1);
          pstate.z = (rd->ux == 0);
          pstate.c = !((uint64_t)operand > rn->ux);
          pstate.v = ((rn->x < 0) && ((int64_t)operand > 0) && (rd->x > 0)) || ((rn->x > 0) && ((int64_t)operand < 0) && (rd->x < 0));
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->uw = rn->uw - (uint32_t)operand;
          pstate.n = bitfield(rd, 31, 1);
          pstate.z = (rd->uw == 0);
          pstate.c = !((uint32_t)operand > rn->uw);
          pstate.v = ((rn->w < 0) && ((int32_t)operand > 0) && (rd->w > 0)) || ((rn->w > 0) && ((int32_t)operand < 0) && (rd->w < 0));
        }
        break;
      }
    }
    else if (C_DPR_OPR_BL(opr)) // bit-logic
    {
      if (sf) // 64-bit
      {
        if (opr & 0b0001) // negated
        {
          operand = ~bitshift64(shift, rm->ux, operand);
        }
        else // not negated
        {
          operand = bitshift64(shift, rm->ux, operand);
        }
      }
      else // 32-bit
      {
        if (opr & 0b0001) // negated
        {
          operand = ~bitshift32(shift, rm->uw, operand);
        }
        else // not negated
        {
          operand = bitshift32(shift, rm->uw, operand);
        }
      }

      switch (opc)
      {
      case DPR_OPC_AND: // and, bic
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
      case DPR_OPC_ORR: // orr, orn
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
      case DPR_OPC_EOR: // eor, eon
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
      case DPR_OPC_ANDS: // ands, bics
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux & operand;
          pstate.n = bitfield(rd, 63, 1);
          pstate.z = (rd->ux == 0);
          pstate.c = 0;
          pstate.v = 0;
        }
        else // 32-bit
        {
          rd->UPPER = 0;
          rd->uw = rn->uw & operand;
          pstate.n = bitfield(rd, 31, 1);
          pstate.z = (rd->uw == 0);
          pstate.c = 0;
          pstate.v = 0;
        }
        break;
      }
    }
  }
  RESET_ZR; // clear zero counter
  PC_INC; // update program counter
}
