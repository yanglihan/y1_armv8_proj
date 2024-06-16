#include "move.h"

#include "stdutil.h"

// converts a wide move operation to binary
instr_t move(char **args, int argc, seg_t opc)
{
  assert(argc == 2 || argc == 3);
  assert(opc >> 2 == 0);

  char *ptr;
  seg_t sf = args[0][0] == 'x';
  seg_t rd = strtol(&args[0][1], &ptr, 10);
  seg_t hw = strtol(&args[1][1], &ptr, 16);
  seg_t opi = 0b101;
  char *immstr;
  strcpy(immstr, args[2]);
  strtok_r(immstr, " ", &immstr);
  seg_t imm16 = strtol(&immstr[1], &ptr, 16);

  return dpi(sf, opc, opi, (hw << 16 | imm16), rd);
}
