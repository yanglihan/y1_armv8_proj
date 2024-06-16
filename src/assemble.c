#include "assemble.h"

const char WZR[] = "w31"; // stack pointer not implemented
const char XZR[] = "x31";

inline instr_t dpi(seg_t sf, seg_t opc, seg_t opi, seg_t operand, seg_t rd) // DP immediate
{
  return (sf << 31) | (opc << 29) | (0b100 << 26) | (opi << 23) | (operand << 5) | rd;
}

inline instr_t dpr(seg_t sf, seg_t opc, seg_t m, seg_t opr, seg_t rm, seg_t operand, seg_t rn, seg_t rd) // DP Register
{
  return (sf << 31) | (opc << 29) | (m << 28) | (0b101 << 25) | (opr << 21) | (rm << 16) | (operand << 10) | (rn << 5) | rd;
}

inline instr_t sdt(seg_t sf, seg_t l, seg_t offset, seg_t xn, seg_t rt) // Single Data Transfer
{
  return (1 << 31) | (sf << 30) | (0b1110010 << 23) | (l << 22) | (offset << 10) | (xn << 5) | rt;
}

inline instr_t ll(seg_t sf, seg_t simm19, seg_t rt) // Load Literal
{
  return (0 << 31) | (sf << 30) | (0b011000 << 24) | (simm19 << 5) | rt;
}

inline instr_t br(seg_t operand) // Branch
{
  return (0b0101 << 26) | operand;
}

instr_t arithmetic(char **args, int lenArgs, bool setFlag, bool ifsub)
{
  assert(lenArgs == 3 || lenArgs == 4);
  char *ptr;
  seg_t sf = args[0][0] == 'x';
  seg_t rd = strtol(&args[0][1], &ptr, 10); // zr case not handled yet (check if & or [] goes first)
  seg_t rn = strtol(&args[1][1], &ptr, 10);
  seg_t opc = (ifsub << 1) | setFlag;

  if(args[2][0] == '#') // case when dpi
  {
    seg_t imm12 = strtol(&args[2][1], &ptr, 16);
    seg_t opi = 0b010;

    if(lenArgs == 4 && strcmp(args[3], "lsl #12") == 0) // check if left shift
    {
      return dpi(sf, opc, opi, (1 << (22 - 5) | imm12 << (10 - 5) | rn), rd);
    }
    else if(lenArgs == 3 || lenArgs == 4)
    {
      return dpi(sf, opc, opi, (0 << (22 - 5) | imm12 << (10 - 5) | rn), rd);
    }
  }
  else // case when dpr
  {
    seg_t m = 0;
    seg_t rm = strtol(&args[2][1], &ptr, 10); // zr case not handled yet
    seg_t opr = 0b1000;

    if(lenArgs == 3)
    {
      return dpr(sf, opc, m, opr, rm, 0, rn, rd);
    }
    else if (lenArgs == 4)
    {
      char *shiftType;
      char *immStr; 
      strcpy(immStr, args[3]);
      shiftType = strtok_r(immStr, " ", &immStr);
      seg_t imm = strtol(&immStr[1], &ptr, 16);

      if(strcmp(shiftType, "lsr") == 0) opr |= 1 << 1;
      if(strcmp(shiftType, "asr") == 0) opr |= 1 << 2;
      
      return dpr(sf, opc, m, opr, rm, imm, rn, rd);
    }
  }

  printf("Invalid Arguments for add/sub commands\n");
  return 0;
}

instr_t logical(char **args, int lenArgs, seg_t opc, seg_t N)
{
  assert(lenArgs == 3 || lenArgs == 4);
  assert(N >> 1 == 0 && opc >> 2 == 0);
  char *ptr;
  seg_t sf = args[0][0] == 'x';
  seg_t rd = strtol(&args[0][1], &ptr, 10);
  seg_t rn = strtol(&args[1][1], &ptr, 10);
  seg_t rm = strtol(&args[2][1], &ptr, 10);
  seg_t m = 0;
  seg_t opr = N;

  if(lenArgs == 3)
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

instr_t wideMove(char **args, int lenArgs, seg_t opc)
{
  assert(lenArgs == 2 || lenArgs == 3);
  assert(opc >> 2 == 0);

  char *ptr;
  seg_t sf = args[0][0] == 'x';
  seg_t rd = strtol(&args[0][1], &ptr, 10);
  seg_t hw = strtol(&args[1][1], &ptr, 16);
  seg_t opi = 0b101;
  char *immStr;
  strcpy(immStr, args[2]);
  strtok_r(immStr, " ", &immStr);
  seg_t imm16 = strtol(&immStr[1], &ptr, 16);

  return dpi(sf, opc, opi, (hw << 16 | imm16), rd);
}

instr_t multiply(char **args, int lenArgs, bool ifsub)
{
  assert(lenArgs == 4);

  char *ptr;
  seg_t sf = args[0][0] == 'x';
  seg_t rd = strtol(&args[0][1], &ptr, 10);
  seg_t rn = strtol(&args[0][1], &ptr, 10);
  seg_t rm = strtol(&args[0][1], &ptr, 10);
  seg_t ra = strtol(&args[0][1], &ptr, 10);
  seg_t m = 1;
  seg_t opr = 0b1000;
  seg_t opc = 0;

  return dpr(sf, opc, m, opr, rm, (ifsub << 5 | ra), rn, rd);
}



int main(int argc, char const *argv[])
{
  /* code */
  // remember to handle rzr case on each instruction
  printf("entered main\n");
  return 0;
}

instr_t branch(char **args, int lenArgs) {
    assert(lenArgs == 1 || lenArgs == 2);
    
    if (lenArgs == 1) {
        // b <literal>
        char *label = args[0];
        int offset = calculate_offset(label);
        return br(offset);
    } else if (lenArgs == 2) {
        if (strcmp(args[0], "br") == 0) {
            // br <Xn>
            char *ptr;
            seg_t xn = strtol(&args[1][1], &ptr, 10);
            return br(xn);
        } else {
            // b.cond <literal>
            char *cond = args[0];
            char *label = args[1];
            int offset = calculate_offset(label);
            seg_t condition = get_condition_code(cond);
            return br_cond(condition, offset);
        }
    }

    printf("Invalid Arguments for branch commands\n");
    return 0;
}

instr_t single_data_transfer(char **args, int lenArgs, bool isLoad) {
    assert(lenArgs == 2 || lenArgs == 3);

    char *ptr;
    seg_t sf = args[0][0] == 'x';
    seg_t rt = strtol(&args[0][1], &ptr, 10);
    seg_t xn = 0;
    seg_t offset = 0;
    seg_t l = isLoad ? 1 : 0;

    if (args[1][0] == '[') {
        char *address = args[1];
        if (sscanf(address, "[x%d]", &xn) == 1) {
            offset = 0;
            return sdt(sf, l, offset, xn, rt);
        } else if (sscanf(address, "[x%d, #%d]!", &xn, &offset) == 2) {
            return sdt(sf, l, offset, xn, rt);
        } else if (sscanf(address, "[x%d], #%d", &xn, &offset) == 2) {
            return sdt(sf, l, offset, xn, rt);
        } else if (sscanf(address, "[x%d, x%d]", &xn, &offset) == 2) {
            return sdt(sf, l, offset, xn, rt);
        } else {
            printf("Invalid Address Format\n");
            return 0;
        }
    } else {
        // Literal address mode
        char *literal = args[1];
        offset = calculate_offset(literal);
        return sdt(sf, l, offset, xn, rt);
    }
}

// Helper function to calculate offsets
int calculate_offset(const char *label) {

    return 0;
}

// Helper function to get condition code
seg_t get_condition_code(const char *cond) {
    if (strcmp(cond, "eq") == 0) return 0b0000;
    if (strcmp(cond, "ne") == 0) return 0b0001;
    if (strcmp(cond, "ge") == 0) return 0b1010;
    if (strcmp(cond, "lt") == 0) return 0b1011;
    if (strcmp(cond, "gt") == 0) return 0b1100;
    if (strcmp(cond, "le") == 0) return 0b1101;
    if (strcmp(cond, "al") == 0) return 0b1110;
    printf("Invalid Condition Code\n");
    return 0;
}

//void dpAssemble(char *command, char **args, FILE *output_FILE)
//{
//  if (strcmp(command, "add") == 0)
//  {
//
//  }
//
//}
