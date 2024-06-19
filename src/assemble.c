
#include "assemble_util/operations.h"
#include "assemble_util/asmconsts.h"
#include "common/datatypes.h"
#include "common/consts.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
  // if (argc != 3)
  // {
  //   printf("Invalid number of arguments!\nExpected: %s input_file [output_file]\n", argv[0]);
  //   return EXIT_SUCCESS;
  // }

  // FILE *in = fopen(argv[1], "rb");
  // FILE *out = fopen(argv[2], "w");
  // if (in == NULL || out == NULL)
  // {
  //   if (in == NULL) printf("Invalid input file!\n");
  //   if (out == NULL) printf("Invalid output file!\n");
  //   fclose(in);
  //   fclose(out);
  //   return EXIT_SUCCESS;
  // }

  // char *lineBuffer[LINE_MAX_LENGTH];

  // while (!feof(in)){
  //   fread(lineBuffer, LINE_MAX_LENGTH, 1, in);
  //   instr_t instruction = (lineBuffer);
  //   if (instruction == 0) continue; // continue on empty lines
  //   if (instruction == NULL) break; // directive not supported yet, exit upon "and x0, x0, x0"
  //   fprintf(out, "%lu", instruction);
  // }

  // fclose(in);
  // fclose(out);

  // return EXIT_SUCCESS;
}
