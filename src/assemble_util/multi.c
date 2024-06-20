#include "multi.h"

#include "arg.h"
#include "asmutil.h"
#include "basics.h"
#include "../common/consts.h"

// converts a multiplication operation to binary
instr_t multi(seg_t *argv, int argc, seg_t neg)
{
  assert(argc == 8);
  seg_t sf = argv[0];
  seg_t rd = argv[1];
  assert(argv[2] == argv[0]);
  seg_t rn = argv[3];
  assert(argv[4] == argv[0]);
  seg_t rm = argv[5];
  assert(argv[6] == argv[0]);
  seg_t ra = argv[7];
  seg_t operand = (neg << 5) | ra;
  return dpr(sf, DEFAULT_NO_ARG, DPR_M_MUL, DPR_OPR_MUL, rm, operand, rn, rd);
}


