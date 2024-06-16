#include "logic.h"

#include "stdutil.h"

instr_t logic(char **args, int argc, seg_t opc, seg_t n)
{
  assert(argc == 3 || argc == 4);
  assert(n >> 1 == 0 && opc >> 2 == 0);
  char *ptr;
  seg_t sf = args[0][0] == 'x';
  seg_t rd = strtol(&args[0][1], &ptr, 10);
  seg_t rn = strtol(&args[1][1], &ptr, 10);
  seg_t rm = strtol(&args[2][1], &ptr, 10);
  seg_t m = 0;
  seg_t opr = n;

  if(argc == 3)
  {
    return dpr(sf, opc, m, opr, rm, 0, rn, rd);
  }
  else
  {
    char *shiftType;
    char *immStr;
    strcpy(immStr, args[3]);
    shiftType = strtok_r(immStr, " ", &immStr);
    seg_t imm = strtol(&immStr[1], &ptr, 16);

    if(strcmp(shiftType, "lsr") == 0) opr |= 1 << 1;
    if(strcmp(shiftType, "asr") == 0) opr |= 1 << 2;
    if(strcmp(shiftType, "ror") == 0) opr |= 0b11 << 1;

    return dpr(sf, opc, m, opr, rm, imm, rn, rd);
  }
}
