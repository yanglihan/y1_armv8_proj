#include "logic.h"

#include "arg.h"
#include "asmutil.h"
#include "basics.h"
#include "../common/consts.h"

// converts a bit-wise logical operation to binary
instr_t logic(seg_t *argv, int argc, seg_t opc, seg_t n)
{
  assert(argc == 6 || argc == 8);
  seg_t sf = argv[0] == ARG_T_REGX;
  seg_t rd = argv[1];
  assert(argv[2] == argv[0]);
  seg_t rn = argv[3];
  assert(argv[4] == argv[0]);
  seg_t rm = argv[5];
  seg_t shift = DEFAULT_NO_ARG;
  seg_t operand = DEFAULT_NO_ARG;
  if (argc == 8) // with shift
  {
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
    case ARG_T_ROR:
      shift = SHIFT_ROR;
      break;
    default:
      assert(0);
      break;
    }
    operand = argv[7];
  }
  seg_t opr = (shift << 1) | n;
  return dpr(sf, opc, DPR_M_NOMUL, opr, rm, operand, rn, rd);
}
