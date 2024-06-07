#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emulate.c"
#define MEM_SIZE 0x200000 // 2MB of memory
// uint8_t mem[MEM_SIZE];

void print_binary(instr num) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
        if(i % 8 == 0) puts("");
    }
    printf("\n");
}

int main()
{
  seg op0;
  instr instr;
  int i = 0;
  while (1) {
    scanf("%xl", &instr);
    mem32_store(i, instr);
    i += 4;
    if(instr == 0x8a000000) break;
  }
  while (1)
  {
    print_state();
    // puts("debug");
    // puts("debug");
    instr = mem32_load(pc.x);
    op0 = take_bits(&instr, 25, 4);
    if (instr == 0x8a000000) // halting
    {
      printf("-- HLT: %08x\n", instr);
      print_binary(instr);
      pc.ux = 0;
      memset(r, 0, sizeof(union reg) * 32);
      memset(mem, 0, MEM_SIZE);
      break;
    }
    else if ((op0 & 0b1110) == 0b1000) // data processing (immediate)
    {
      printf("-- DPI: %08x\n", instr);
      print_binary(instr);
      dpi(instr);
    }
    else if ((op0 & 0b0111) == 0b0101) // data processing (register)
    {
      printf("-- DPR: %08x\n", instr);
      print_binary(instr);
      dpr(instr);
    }
    else if ((op0 & 0b0101) == 0b0100) // loads and stores
    {
      printf("-- LS : %08x\n", instr);
      print_binary(instr);
      ls(instr);
    }
    else if ((op0 & 0b1110) == 0b1010) // branches
    {
      printf("-- BR : %08x\n", instr);
      print_binary(instr);
      br(instr);
    }
    else // unknown command
    {
      printf("-- UNK: %08x\n", instr);
      print_binary(instr);
      break;
    }
    if((op0 & 0b1110) != 0b1010)
      pc.ux += 4;
  }
  return 0;
}

void print_state()
{
  printf("Registers:\n");
  for (int i = 0; i < 31; i++)
  {
    printf("X%02d    = %016llx\n", i, r[i].ux);
  }
  printf("PC     = %016llx\n", pc.ux);
  printf("PSTATE : %c%c%c%c\n", pstate.n ? 'N' : '-', pstate.z ? 'Z' : '-', pstate.c ? 'C' : '-', pstate.v ? 'V' : '-');
  printf("Non-zero memory:\n");
  for (int64_t i = 0; i < MEM_SIZE; i += 4)
  {
    uint64_t m = mem32_load(i);
    if (!m)
    {
      continue;
    }
    printf("%x: %x\n", i, m);
  }
}
