#include "move.h"

#include "../common/consts.h"
#include "arg.h"
#include "asmutil.h"
#include "basics.h"

// converts a wide move operation to binary
instr_t move(int *argv, int argc, seg_t opc)
{
  assert(argc == 4 || argc == 6);
  seg_t sf = argv[0] == ARG_T_REGX;
  seg_t rd = argv[1];
  assert(argv[2] == ARG_T_IMM);
  seg_t imm16 = argv[3];
  seg_t hw = argc == 6;
  if (argc == 6)
  {
    assert(argv[4] == ARG_T_LSL);
  }
  seg_t operand = (hw << 16) | imm16;
  return dpi(sf, opc, DPI_OPI_WM, operand, rd);\
}
