#include "op_ls.h"

#include "bitwise.h"
#include "memory.h"
#include "state.h"

// loads and stores
void ls(instr_t instr)
{
  seg_t rti = take_bits(&instr, 0, 5);
  seg_t xni = take_bits(&instr, 5, 5);
  seg_t sf = take_bits(&instr, 30, 1);
  seg_t load = take_bits(&instr, 22, 1);
  seg_t addr;
  int64_t offset;
  reg_t* rt = r + rti;
  reg_t* xn = r + xni;
  seg_t simm19 = take_bits(&instr, 5, 19);
  seg_t imm12 = take_bits(&instr, 10, 12);
  seg_t xmi = take_bits(&instr, 16, 5);
  reg_t* xm = r + xmi;
  seg_t simm9 = take_bits(&instr, 12, 9);
  seg_t i = take_bits(&instr, 11, 1);

  if (!take_bits(&instr, 31, 1)) // load literal
  {
    offset = sgnext64(simm19, 45);
    addr = pc.x + offset * 4;
    load = 1;
  }
  else // single data transfer
  {
    if (take_bits(&instr, 24, 1)) // unsigned
    {
      offset = sgnext64(imm12, 52);
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
      offset = sgnext64(simm9, 55);
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
  RESET_ZR; // clear zero counter
  PC_INC; // update program counter
}
