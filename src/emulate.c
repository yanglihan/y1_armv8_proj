#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define zr r[31]
#define Cur(x) ((cur >> (x)) & 1)
#define cur(l, r) ((cur >> (l)) & ((1ULL << ((r) - (l) + 1)) - 1))
#define MEM_SIZE 0x200000 // 2MB of memory
uint8_t memory[MEM_SIZE];

void init_memory()
{
    memset(memory, 0, MEM_SIZE);
}

uint64_t load_memory_64(uint64_t address)
{
    if (address + 7 >= MEM_SIZE)
    {
        return 0;
    }
    uint64_t value = 0;
    for (int i = 0; i < 8; i++)
    {
        value |= ((uint64_t)memory[address + i]) << (8 * i);
    }
    return value;
}
void store_memory_64(uint64_t address, uint64_t data)
{
    if (address + 7 >= MEM_SIZE)
    {
        return;
    }
    for (int i = 0; i < 8; i++)
    {
        memory[address + i] = (data >> (8 * i)) & 0xFF;
    }
}
uint32_t load_memory_32(uint64_t address)
{
    if (address + 3 >= MEM_SIZE)
    {
        return 0;
    }
    uint32_t value = 0;
    for (int i = 0; i < 4; i++)
    {
        value |= ((uint32_t)memory[address + i]) << (8 * i);
    }
    return value;
}

void store_memory_32(uint64_t address, uint32_t data)
{
    if (address + 3 >= MEM_SIZE)
    {
        return;
    }
    for (int i = 0; i < 4; i++)
    {
        memory[address + i] = (data >> (8 * i)) & 0xFF;
    }
}

typedef uint32_t instr;
typedef uint32_t seg;

union reg
{
    int64_t x;
    int32_t w;
    uint64_t ux;
    uint32_t uw;
    uint32_t upper[2];
} r[32], pc, sp;

struct
{
    bool n, z, c, v;
} pstate = {0, 1, 0, 0};

void Single_data_transfer(instr cur)
{
    seg rt = cur(0, 4);
    seg xn = cur(5, 9);
    seg sf = Cur(30);
    seg load = Cur(22);
    seg target_address;
    if (Cur(31) == 0)
    { // check type of transform
        target_address = pc.x + cur(5, 23) * 4;
        load = 1;
    }
    else if (Cur(24))
    { // check unsigned
        seg imm12 = cur(10, 21);
        if (Cur(31) == 1)
        {
            target_address = r[xn].uw + imm12 * 4;
        }
        else
        {
            target_address = r[xn].ux + imm12 * 8;
        }
    }
    else if (Cur(21) == 1)
    { // register offset
        seg xm = cur(16, 20);
        if (Cur(31) == 1)
        {
            target_address = r[xn].uw + xm;
        }
        else
        {
            target_address = r[xn].ux + xm;
        }
    }
    else
    { // check pre/post
        seg simm9 = cur(12, 20);
        if (Cur(31) == 1)
        {
            target_address = r[xn].uw + ((Cur(11) == 1) ? simm9 : 0);
            r[xn].uw += simm9;
        }
        else
        {
            target_address = r[xn].ux + ((Cur(11) == 1) ? simm9 : 0);
            r[xn].ux += simm9;
        }
    }
    if (load == 1)
    { // load / store
        if (sf == 0)
        {
            r[rt].uw = load_memory_32(target_address);
        }
        else
        {
            r[rt].ux = load_memory_64(target_address);
        }
    }
    else
    {
        if (sf == 0)
        {
            store_memory_32(target_address, r[xn].uw);
        }
        else
        {
            store_memory_64(target_address, r[xn].ux);
        }
    }
}

void branch(instr cur)
{
    seg kind = cur(21, 30);
    if (kind == 0)
    { // unconditional
        seg simm26 = cur(0, 25);
        int64_t offset = ((int64_t)simm26 << 38) >> 38;
        offset *= 4;
        pc.x += offset;
    }
    else if (kind == 1)
    { // conditional
        seg simm19 = cur(5, 23);
        int64_t offset = ((int64_t)simm19 << 38) >> 38;
        offset *= 4;
        seg cond = cur(0, 3);
        bool check = false;
        switch (cond)
        {
        case 0:
            check = pstate.z == 1;
            break;
        case 1:
            check = pstate.z == 0;
            break;
        case 10:
            check = pstate.n == pstate.v;
            break;
        case 11:
            check = pstate.n != pstate.v;
            break;
        case 12:
            check = pstate.z == 0 && pstate.n == pstate.v;
            break;
        case 13:
            check = !(pstate.z == 0 && pstate.n == pstate.v);
            break;
        case 14:
            check = true;
            break;
        }
        if(check) {
            pc.x += offset;
        }
    }
    else if (kind == 3) // register
    { 
        seg xn = cur(5,9);
        pc = r[xn];
    }
}


int main(int argc, char **argv)
{
  char *input_filename, *output_filename;
  if (argc == 1)
  {
    input_filename = argv[0];
  }
  else if (argc == 2)
  {
    input_filename = argv[0];
    output_filename = argv[1];
  }
  else
  {
    return EXIT_FAILURE;
  }

  
  FILE *input_file = fopen(input_filename, "rb");
  instr buffer;
  seg op0;

  while(!feof(input_file)) {
    fread(buffer, sizeof(buffer), 1, input_file);
    op0 = take_bits(buffer, 25, 4);
    if (op0 & 0b1110 == 0b1000) // data processing (immediate)
    {
      dpi(buffer);
    }
    else if (op0 & 0b0111 == 0b0101) // data processing (register)
    {
      dpr(buffer);
    }
    else if (op0 & 0b0101 == 0b0100) // loads and stores
    {
      ls(buffer);
    }
    else if (op0 & 0b1110 == 0b1010) // branches
    {
      br(buffer);
    }
    else if (op0 == 0x8a000000) // halting
    {
      break;
    }
  }
  fclose(input_file);

  if (output_filename)
  {
    FILE *output_file = fopen(output_filename, "w");
    fprintf("Registers:\n", output_file);
    for (int i = 0; i < 31; i++)
    {
      printf("X%02d    = %016x\n", i, r[i].x);
    }
    printf("PC     = %016x\n", pc.x);
    printf("PSTATE : %c%c%c%c\n", pstate.n ? 'N' : '-', pstate.z ? 'Z' : '-', pstate.c ? 'C' : '-', pstate.v ? 'V' : '-');
    printf("Non-zero memory:\n");
    // TODO: scan non-zero memory
  }
  else
  {
    // TODO: terminal output in the above format
  }
  

  return EXIT_SUCCESS;
}

uint64_t take_bits(void *from, int begin, int size)
{
  uint64_t i = *(uint64_t *)from;
  i >>= begin;
  i &= (1 << size) - 1;
  return i;
}

void dpi(instr instr)
{
  seg sf = take_bits(&instr, 31, 1);
  seg opc = take_bits(&instr, 29, 2);
  seg opi = take_bits(&instr, 26, 3);
  seg operand = take_bits(&instr, 5, 18);
  seg rdi = take_bits(&instr, 0, 5);
  union reg *rd = r + rdi;
  seg sh = take_bits(&instr, 22, 1);
  seg imm12 = take_bits(&instr, 10, 12) << (sh * 12);
  seg rni = take_bits(&instr, 5, 5);
  union reg *rn = r + rni;
  seg hw = take_bits(&instr, 21, 2);
  seg imm16 = take_bits(&instr, 5, 16) << (hw * 16);

  switch (opi)
  {
  case 0b010: // arithmatic operation
    switch (opc)
    {
    case 0b00: // add
      if (sf)  // 64-bit
      {
        rd->x = rn->x + (int64_t)imm12;
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->w = rn->w + (int32_t)imm12;
      }
      break;
    case 0b01: // adds
      if (sf)  // 64-bit
      {
        rd->x = rn->x + (int64_t)imm12;
        pstate.n = take_bits(rd, 63, 1);
        pstate.z = (rd->ux == 0);
        pstate.c = (UINT64_MAX - imm12 > rn->ux);
        pstate.v = (INT64_MAX - (int64_t)imm12 > rn->x);
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->w = rn->w + (int32_t)imm12;
        pstate.n = take_bits(rd, 31, 1);
        pstate.z = (rd->uw == 0);
        pstate.c = (UINT32_MAX - imm12 > rn->uw);
        pstate.v = (INT32_MAX - (int32_t)imm12 > rn->w);
      }
      break;
    case 0b10: // sub
      if (sf)  // 64-bit
      {
        rd->x = rn->x - (int64_t)imm12;
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->w = rn->w - (int32_t)imm12;
      }
      break;
    case 0b11: // subs
      if (sf)  // 64-bit
      {
        rd->x = rn->x - (int64_t)imm12;
        pstate.n = take_bits(rd, 63, 1);
        pstate.z = (rd->ux == 0);
        pstate.c = (imm12 > rn->ux);
        pstate.v = (INT64_MIN + (int64_t)imm12 > rn->x);
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->w = rn->w - (int32_t)imm12;
        pstate.n = take_bits(rd, 31, 1);
        pstate.z = (rd->uw == 0);
        pstate.c = (imm12 > rn->uw);
        pstate.v = (INT32_MIN + (int32_t)imm12 > rn->w);
      }
      break;
    }
    break;
  case 0b101: // wide move operation
    switch (opc)
    {
    case 0b00: // movn
      if (sf)  // 64-bit
      {
        rd->ux = !(uint64_t)imm16;
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->uw = !(uint32_t)imm16;
      }
      break;
    case 0b10: // movz
      if (sf)  // 64-bit
      {
        rd->ux = (uint64_t)imm16;
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->uw = (uint32_t)imm16;
      }
      break;
    case 0b11: // movk
      if (sf)  // 64-bit
      {
        rd->ux = (uint64_t)imm16 | (rd->ux & !((uint64_t)0xffff << (hw * 16)));
      }
      else // 32-bit
      {
        rd->upper[1] = 0;
        rd->uw = (uint32_t)imm16 | (rd->uw & !((uint32_t)0xffff << (hw * 16)));
      }
      break;
    }
    break;
  }
  zr->uw = 0;
}

void dpr(instr instr)
{
  seg sf = take_bits(&instr, 31, 1);
  seg opc = take_bits(&instr, 29, 2);
  seg m = take_bits(&instr, 28, 1);
  seg opr = take_bits(&instr, 21, 4);
  seg rmi = take_bits(&instr, 16, 5);
  seg operand = take_bits(&instr, 10, 6);
  seg rni = take_bits(&instr, 5, 5);
  seg rdi = take_bits(&instr, 0, 4);
  seg rai = take_bits(&instr, 10, 5);
  union reg *rm = r + rmi;
  union reg *rn = r + rni;
  union reg *rd = r + rdi;
  union reg *ra = r + rai;
  int64_t imm;

  if (m)
  {
    if (opr == 0b1000) // multiply
    {
      if (!(operand & 0b100000)) // madd
      {
        if (sf) // 64-bit
        {
          rd->x = ra->x + rn->x * rm->x;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = ra->w + rn->w * rm->w;
        }
      }
      else // msub
      {
        if (sf) // 64-bit
        {
          rd->x = ra->x - rn->x * rm->x;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = ra->w - rn->w * rm->w;
        }
      }
    }
  }
  else
  {
    if (opr & 0b1001 == 0b1000) // arithmatic
    {
      if (sf) // 64-bit
      {
        imm = bit_shift64(opr, rm, operand);
      }
      else // 32-bit
      {
        imm = bit_shift32(opr, rm, operand);
      }
      switch (opc)
      {
      case 0b00: // add
        if (sf)  // 64-bit
        {
          rd->x = rn->x + (int64_t)imm;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = rn->w + (int32_t)imm;
        }
        break;
      case 0b01: // adds
        if (sf)  // 64-bit
        {
          rd->x = rn->x + (int64_t)imm;
          pstate.n = take_bits(rd, 63, 1);
          pstate.z = (rd->ux == 0);
          pstate.c = (UINT64_MAX - imm > rn->ux);
          pstate.v = (INT64_MAX - (int64_t)imm > rn->x);
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = rn->w + (int32_t)imm;
          pstate.n = take_bits(rd, 31, 1);
          pstate.z = (rd->uw == 0);
          pstate.c = (UINT32_MAX - imm > rn->uw);
          pstate.v = (INT32_MAX - (int32_t)imm > rn->w);
        }
        break;
      case 0b10: // sub
        if (sf)  // 64-bit
        {
          rd->x = rn->x - (int64_t)imm;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = rn->w - (int32_t)imm;
        }
        break;
      case 0b11: // subs
        if (sf)  // 64-bit
        {
          rd->x = rn->x - (int64_t)imm;
          pstate.n = take_bits(rd, 63, 1);
          pstate.z = (rd->ux == 0);
          pstate.c = (imm > rn->ux);
          pstate.v = (INT64_MIN + (int64_t)imm > rn->x);
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->w = rn->w - (int32_t)imm;
          pstate.n = take_bits(rd, 31, 1);
          pstate.z = (rd->uw == 0);
          pstate.c = (imm > rn->uw);
          pstate.v = (INT32_MIN + (int32_t)imm > rn->w);
        }
        break;
      }
    }
    else if (opr & 0b1000 == 0b0000) // bit-logic
    {
      if (sf) // 64-bit
      {
        if (opr & 0b0001) // negated
        {
          imm = ~bit_shift64(opr, rm, operand);
        }
        else // not negated
        {
          imm = bit_shift64(opr, rm, operand);
        }
      }
      else // 32-bit
      {
        if (opr & 0b0001) // negated
        {
          imm = ~bit_shift32(opr, rm, operand);
        }
        else // not negated
        {
          imm = bit_shift32(opr, rm, operand);
        }
      }

      switch (opc)
      {
      case 0b00: // and, bic
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux & imm;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->uw = rn->uw & imm;
        }
        break;
      case 0b01: // orr, orn
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux | imm;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->uw = rn->uw | imm;
        }
        break;
      case 0b10: // eor, eon
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux ^ imm;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->uw = rn->uw ^ imm;
        }
        break;
      case 0b11: // ands, bics
        if (sf)  // 64-bit
        {
          rd->ux = rn->ux & imm;
          pstate.n = take_bits(rd, 63, 1);
          pstate.z = (rd->ux == 0);
          pstate.c = 0;
          pstate.v = 0;
        }
        else // 32-bit
        {
          rd->upper[1] = 0;
          rd->uw = rn->uw & imm;
          pstate.n = take_bits(rd, 31, 1);
          pstate.z = (rd->uw == 0);
          pstate.c = 0;
          pstate.v = 0;
        }
        break;
      }
    }
  }
}

uint32_t bit_shift32(seg opr, seg oprand, int shift_amount)
{
  switch (opr & 0b0110)
  {
  case 0b0000: // lsl
    return oprand << shift_amount;
    break;
  case 0b0010: // lsr
    return oprand >> shift_amount;
    break;
  case 0b0100: // asr
    return (oprand >> shift_amount) | (take_bits(&oprand, 31, 1) * (UINT32_MAX << (32 - shift_amount)));
    break;
  case 0b0110: // ror
    return (oprand >> shift_amount) | (oprand << (32 - shift_amount));
    break;
  }
  return 0;
}

uint64_t bit_shift64(seg opr, uint64_t oprand, int shift_amount)
{
  switch (opr & 0b0110)
  {
  case 0b0000: // lsl
    return oprand << shift_amount;
    break;
  case 0b0010: // lsr
    return oprand >> shift_amount;
    break;
  case 0b0100: // asr
    return (oprand >> shift_amount) | (take_bits(oprand, 63, 1) * (UINT64_MAX << (64 - shift_amount)));
    break;
  case 0b0110: // ror
    return (oprand >> shift_amount) | (oprand << (64 - shift_amount));
    break;
  }
  return 0;
}


void ls(instr instr);
void br(instr instr);

