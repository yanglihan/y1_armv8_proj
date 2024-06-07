#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emulate.c"
#define MEM_SIZE 0x200000 // 2MB of memory
// uint8_t mem[MEM_SIZE];

int main(int argc, char **argv)
{
  printf("argc: %d\n", argc);
  if (argc == 2) // terminal output
  {
    // input_filename = malloc(strlen(argv[1]));
    printf("Input file: %s\n", argv[1]);
    // strcpy(input_filename, argv[1]);
  }
  else if (argc == 3) // file output
  {
    // input_filename = malloc(strlen(argv[1]));
    // output_filename = malloc(strlen(argv[2]));
    printf("Input file: %s\n", argv[1]);
    printf("Output file: %s\n", argv[2]);
    // strcpy(input_filename, argv[1]);
    // strcpy(output_filename, argv[2]);
  }
  else
  {
    printf("Invalid number of arguments!\nExpected: emulate input_file [output_file]\n");
    return EXIT_FAILURE;
  }

  FILE *input_file = fopen(argv[1], "rb");
  uint8_t *buffer;
  seg op0;

  if (input_file == NULL) // invalid source file
  {
    printf("Invalid input file!\n");
    return EXIT_FAILURE;
  }

  fread(mem, MEM_SIZE, 1, input_file);
  fclose(input_file);

  while (1)
  {
    printf("PC: %08x\n", pc.uw);
    instr instr = mem32_load(pc.ux);
    op0 = take_bits(&instr, 25, 4);
    if (instr == 0x8a000000) // halting
    {
      printf("-- HLT: %08x\n", instr);
      break;
    }
    if ((op0 & 0b1110) == 0b1000) // data processing (immediate)
    {
      printf("-- DPI: %08x\n", instr);
      dpi(instr);
    }
    else if ((op0 & 0b0111) == 0b0101) // data processing (register)
    {
      printf("-- DPR: %08x\n", instr);
      dpr(instr);
    }
    else if ((op0 & 0b0101) == 0b0100) // loads and stores
    {
      printf("-- LS : %08x\n", instr);
      ls(instr);
    }
    else if ((op0 & 0b1110) == 0b1010) // branches
    {
      printf("-- BR : %08x\n", instr);
      br(instr);
    }
    else // unknown command
    {
      printf("-- UNK: %08x\n", instr);
      break;
    }
    if((op0 & 0b1110) != 0b1010) // if not branches pc add 4
      pc.ux += 4;
  }

  if (argc == 3)
  {
    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL)
    {
      printf("Invalid output file!");
      return EXIT_FAILURE;
    }
    fprintf(output_file, "Registers:\n");
    for (int i = 0; i < 31; i++)
    {
      fprintf(output_file, "X%02d    = %016llx\n", i, r[i].ux);
    }
    fprintf(output_file, "PC     = %016llx\n", pc.ux);
    fprintf(output_file, "PSTATE : %c%c%c%c\n", pstate.n ? 'N' : '-', pstate.z ? 'Z' : '-', pstate.c ? 'C' : '-', pstate.v ? 'V' : '-');
    fprintf(output_file, "Non-Zero Memory:\n");
    for (uint64_t i = 0; i < MEM_SIZE; i += 4)
    {
      uint64_t m = mem32_load(i);
      if (!m)
      {
        continue;
      }
      fprintf(output_file, "%#010llx: %#010llx\n", i, m);
    }
    fclose(output_file);
  }
  else
  {
    printf("Registers:\n");
    for (int i = 0; i < 31; i++)
    {
      printf("X%02d    = %016llx\n", i, r[i].ux);
    }
    printf("PC     = %016llx\n", pc.ux);
    printf("PSTATE : %c%c%c%c\n", pstate.n ? 'N' : '-', pstate.z ? 'Z' : '-', pstate.c ? 'C' : '-', pstate.v ? 'V' : '-');
    printf("Non-Zero Memory:\n");
    for (int64_t i = 0; i < MEM_SIZE; i += 4)
    {
      uint64_t m = mem32_load(i);
      if (!m)
      {
        continue;
      }
      printf("%#010llx: %#010llx\n", i, m);
    }  
  }

  return EXIT_SUCCESS;
}
