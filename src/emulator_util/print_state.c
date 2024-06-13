#include "print_state.h"

// print all registers and non-zero memories to terminal
void print_state(void)
{
  printf("Registers:\n");
  for (int i = 0; i < 31; i++)
  {
    printf("X%02d    = %016llx\n", i, r[i].ux); // e.g. "X05    = 00000000ffffffc4\n"
  }
  printf("PC     = %016x\n", PC);             // e.g. "PC     = 00000000ffffffc4\n"
  printf("PSTATE : %c%c%c%c\n", pstate.n ? 'N' : '-', pstate.z ? 'Z' : '-', pstate.c ? 'C' : '-', pstate.v ? 'V' : '-');
  // e.g. "PSTATE : Z-C-\n"
  printf("Non-zero memory:\n");
  for (int64_t i = 0; i < MEM_SIZE; i += 4)
  {
    uint64_t m = mem32_load(i);
    if (m) // non-zero memory
    {
      printf("%#010llx: %08llx\n", i, m);         // e.g. "0x00000004: 00a30b92\n"
    }
  }
}

// print all registers and non-zero memories to a file
void fprint_state(FILE* out)
{
  fprintf(out, "Registers:\n");
  for (int i = 0; i < 31; i++)
  {
    fprintf(out, "X%02d    = %016llx\n", i, r[i].ux); // e.g. "X05    = 00000000ffffffc4\n"
  }
  fprintf(out, "PC     = %016x\n", PC);             // e.g. "PC     = 00000000ffffffc4\n"
  fprintf(out, "PSTATE : %c%c%c%c\n", pstate.n ? 'N' : '-', pstate.z ? 'Z' : '-', pstate.c ? 'C' : '-', pstate.v ? 'V' : '-');
  // e.g. "PSTATE : Z-C-\n"
  fprintf(out, "Non-zero memory:\n");
  for (uint64_t i = 0; i < MEM_SIZE; i += 4)
  {
    uint64_t m = mem32_load(i);
    if (!m)
    {
      continue;
    }
    fprintf(out, "%#010llx: %08llx\n", i, m);         // e.g. "0x00000004: 00a30b92\n"
  }
}
