#include "arith.h"

#include "asmutil.h"

// converts an arithmetic operation to binary
instr_t arith(char **args, int argc, int8_t flg, int8_t sub)
{
  assert(argc == 3 || argc == 4);
  char *ptr;
  seg_t sf = args[0][0] == 'x';
  seg_t rd = strtol(&args[0][1], &ptr, 10); // zr case not handled yet (check if & or [] goes first)
  seg_t rn = strtol(&args[1][1], &ptr, 10);
  seg_t opc = (sub << 1) | flg;

  if(args[2][0] == '#') // case when dpi
  {
    seg_t imm12 = strtol(&args[2][1], &ptr, 16);
    seg_t opi = 0b010;

    if(argc == 4 && strcmp(args[3], "lsl #12") == 0) // check if left shift
    {
      return dpi(sf, opc, opi, (1 << (22 - 5) | imm12 << (10 - 5) | rn), rd);
    }
    else if(argc == 3 || argc == 4)
    {
      return dpi(sf, opc, opi, (0 << (22 - 5) | imm12 << (10 - 5) | rn), rd);
    }
  }
  else // case when dpr
  {
    seg_t m = 0;
    seg_t rm = strtol(&args[2][1], &ptr, 10); // zr case not handled yet
    seg_t opr = 0b1000;

    if(argc == 3)
    {
      return dpr(sf, opc, m, opr, rm, 0, rn, rd);
    }
    else if (argc == 4)
    {
      char *shift;
      char *immStr; 
      strcpy(immStr, args[3]);
      shift = strtok_r(immStr, " ", &immStr);
      seg_t imm = strtol(&immStr[1], &ptr, 16);

      if(strcmp(shift, "lsr") == 0) opr |= 1 << 1;
      if(strcmp(shift, "asr") == 0) opr |= 1 << 2;
      
      return dpr(sf, opc, m, opr, rm, imm, rn, rd);
    }
  }

  assert(0);
  // printf("Invalid Arguments for add/sub commands\n");
  return 0;
}
