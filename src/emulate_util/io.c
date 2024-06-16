#include "io.h"

#include "memory.h"
#include "state.h"
#include <stdio.h>

// print all registers and non-zero memories to terminal
static void tprintstate(void)
{
  printf("Registers:\n");
  for (int i = 0; i < 31; i++)
  {
    printf("X%02d    = %016llx\n", i, r[i].ux); // e.g. "X05    = 00000000ffffffc4\n"
  }
  printf("PC     = %016x\n", PC);               // e.g. "PC     = 00000000ffffffc4\n"
  printf("PSTATE : %c%c%c%c\n", P_FLG_N, P_FLG_Z, P_FLG_C, P_FLG_V);
                                                // e.g. "PSTATE : Z-C-\n"
  printf("Non-zero memory:\n");
  for (int64_t i = 0; i < MEM_SIZE; i += 4)
  {
    uint64_t m = mload32(i);
    if (m) // non-zero memory
    {
      printf("%#010llx: %08llx\n", i, m);       // e.g. "0x00000004: 00a30b92\n"
    }
  }
}

// print all registers and non-zero memories to a file
static void fprintstate(FILE* out)
{
  fprintf(out, "Registers:\n");
  for (int i = 0; i < 31; i++)
  {
    fprintf(out, "X%02d    = %016llx\n", i, r[i].ux); // e.g. "X05    = 00000000ffffffc4\n"
  }
  fprintf(out, "PC     = %016x\n", PC);               // e.g. "PC     = 00000000ffffffc4\n"
  fprintf(out, "PSTATE : %c%c%c%c\n",  P_FLG_N, P_FLG_Z, P_FLG_C, P_FLG_V);
                                                      // e.g. "PSTATE : Z-C-\n"
  fprintf(out, "Non-zero memory:\n");
  for (uint64_t i = 0; i < MEM_SIZE; i += 4)
  {
    uint64_t m = mload32(i);
    if (!m)
    {
      continue;
    }
    fprintf(out, "%#010llx: %08llx\n", i, m);         // e.g. "0x00000004: 00a30b92\n"
  }
}

// write to terminal, always returns success
int tout(void) {
  tprintstate();
  return IO_SUCCESS;
}

// write to file, returns failure if output file is invalid
int fout(char* path)
{
  FILE* out = fopen(path, "w");
  if (out == NULL) // invalid output file
  {
    return IO_FAILURE;
  }
  fprintstate(out);
  fclose(out);
  return IO_SUCCESS;
}

// load a program to memory, returns failure if input file is invalid
int fload(char* path)
{
  FILE* in = fopen(path, "rb");
  if (in == NULL) // invalid source file
  {
    return IO_FAILURE;
  }
  memfread(in);
  fclose(in);
  return IO_SUCCESS;
}
