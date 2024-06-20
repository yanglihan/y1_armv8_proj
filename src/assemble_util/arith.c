#include "arith.h"

#include "arg.h"
#include "asmutil.h"
#include "basics.h"
#include "../common/consts.h"

// converts an arithmetic operation to binary
instr_t arith(seg_t *argv, int argc, seg_t opc)
{
  assert(argc == 6 || argc == 8);

  seg_t sf = argv[0] == ARG_T_REGX;
  seg_t rd = argv[1];
  if (argv[4] == ARG_T_IMM) // dpi
  {
    seg_t imm12 = argv[5];
    seg_t opi = DPI_OPI_AR;
    seg_t sh = DPI_SH_NOSHIFT;
    assert(argv[2] == argv[0]);
    seg_t rn = argv[3];
    if (argc == 8) // with shift
    {
      assert(argv[6] == ARG_T_LSL);
      sh = DPI_SH_SHIFT * (argv[7] == 12);
    }
    return dpi(sf, opc, opi, (sh << 17) | (imm12 << 5) | rn, rd);
  }
  else // dpr
  {
    seg_t m = DPR_M_NOMUL;
    assert(argv[2] == argv[0]);
    seg_t rn = argv[3];
    assert(argv[4] == argv[0]);
    seg_t rm = argv[5];
    seg_t opr = DPR_OPR_ARITH;
    seg_t operand = 0;
    if (argc == 8) // with shift
    {
      int shift = SHIFT_LSL;
      switch (argv[6])
      {
      case ARG_T_LSL:
        shift = SHIFT_LSL;
        break;
      case ARG_T_LSR:
        shift = SHIFT_LSR;
        break;
      case ARG_T_ASR:
        shift = SHIFT_ASR;
        break;
      default:
        assert(0);
        break;
      }
      opr |= shift << 1;
      operand = argv[7];
    }
    return dpr(sf, opc, m, opr, rm, operand, rn, rd);
  }
  assert(0);
  return 0;
}
