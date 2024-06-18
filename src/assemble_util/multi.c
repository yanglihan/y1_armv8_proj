#include "multi.h"

#include "asmutil.h"

// converts a multiplication operation to binary
instr_t multi(char **args, int argc, int8_t sub)
{
  assert(argc == 4);

  char *ptr;
  seg_t sf = args[0][0] == 'x';
  seg_t rd = strtol(&args[0][1], &ptr, 10);
  seg_t rn = strtol(&args[0][1], &ptr, 10);
  seg_t rm = strtol(&args[0][1], &ptr, 10);
  seg_t ra = strtol(&args[0][1], &ptr, 10);
  seg_t m = 1;
  seg_t opr = 0b1000;
  seg_t opc = 0;

  return dpr(sf, opc, m, opr, rm, (sub << 5 | ra), rn, rd);
}


