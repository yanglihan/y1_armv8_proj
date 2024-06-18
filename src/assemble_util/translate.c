#include "translate.h"
#include "stdutil.h"
#include "operations.h"


const int MAXNUMPARAMETER = 4;
const char WZR[] = "w31"; // stack pointer not implemented
const char XZR[] = "x31";

instr_t translate(char command[LINELENGTH]){

  char *opcode;
  char **argv;
  int *argc = 0;
  char str[LINELENGTH];

  int spaceDelim = 0;
  while (command[spaceDelim] == ' ') spaceDelim++; //eliminate preceeding spaces
  strcpy(str, command+spaceDelim);

  if (str[0] == '\n') return 0; // exit current line if it's entirely made of spaces

  opcode = strtok_r(str, " ", &str);
  for (int i = 0; i < MAXNUMPARAMETER; i++) 
  {
    spaceDelim = 0;
    while (str[spaceDelim] == ' ') spaceDelim++; //eliminate preceeding spaces
    strcpy(str, str+spaceDelim);

    char *token = strtok_r(str, ", ", &str);
    if (token == NULL) break;

    argv[i] = token;
    argc++;
  }

  if(strcmp(opcode, "and") == 0 && 
     strcmp(tolower(argv[0]), "x0") == 0 &&
     strcmp(tolower(argv[1]), "x0") == 0 &&
     strcmp(tolower(argv[2]), "x0") == 0)
  {
    return NULL;
  }

  if (strcmp(opcode, "add") == 0)
  {
    return arith(argv, argc, 0, 0);
  }
  else if (strcmp(opcode, "adds") == 0)
  {
    return arith(argv, argc, 1, 0);
  }
  else if (strcmp(opcode, "sub") == 0)
  {
    return arith(argv, argc, 0, 1);
  }
  else if (strcmp(opcode, "subs") == 0)
  {
    return arith(argv, argc, 1, 1);
  }
  else if (strcmp(opcode, "cmp") == 0)
  {
    insertZR(argv, argc, 0);
    return arith(argv, argc, 1, 1);
  }
  else if (strcmp(opcode, "cmn") == 0)
  {
    insertZR(argv, argc, 0);
    return arith(argv, argc, 1, 0);
  }
  else if (strcmp(opcode, "neg") == 0)
  {
    insertZR(argv, argc, 1);
    return arith(argv, argc, 0, 1);
  }
  else if (strcmp(opcode, "negs") == 0)
  {
    insertZR(argv, argc, 1);
    return arith(argv, argc, 1, 1);
  }
  else if (strcmp(opcode, "and") == 0)
  {
    return logic(argv, argc, 0b00, 0);
  }
  else if (strcmp(opcode, "ands") == 0)
  {
    return logic(argv, argc, 0b11, 0);
  }
  else if (strcmp(opcode, "bic") == 0)
  {
    return logic(argv, argc, 0b00, 1);
  }
  else if (strcmp(opcode, "bics") == 0)
  {
    return logic(argv, argc, 0b11, 1);
  }
  else if (strcmp(opcode, "eor") == 0)
  {
    return logic(argv, argc, 0b10, 0);
  }
  else if (strcmp(opcode, "eon") == 0)
  {
    return logic(argv, argc, 0b10, 1);
  }
  else if (strcmp(opcode, "orr") == 0)
  {
    return logic(argv, argc, 0b01, 0);
  }
  else if (strcmp(opcode, "orn") == 0)
  {
    return logic(argc, argv, 0b01, 1);
  }
  else if (strcmp(opcode, "tst") == 0)
  {
    insertZR(argv, argc, 0);
    return logic(argv, argc, 0b11, 0);
  }
  else if (strcmp(opcode, "mvn") == 0)
  {
    insertZR(argv, argc, 1);
    return logic(argv, argc, 0b01, 1);
  }
  else if (strcmp(opcode, "mov") == 0)
  {
    insertZR(argv, argc, 1);
    return logic(argv, argc, 0b01, 0);
  }
  else if (strcmp(opcode, "movn") == 0)
  {
    return move(argv, argc, 0b00);
  }
  else if (strcmp(opcode, "movk") == 0)
  {
    return move(argv, argc, 0b11);
  }
  else if (strcmp(opcode, "movz") == 0)
  {
    return move(argv, argc, 0b10);
  }
  else if (strcmp(opcode, "madd") == 0)
  {
    return multi(argv, argc, 0);
  }
  else if (strcmp(opcode, "msub") == 0)
  {
    return multi(argv, argc, 1);
  }
  else if (strcmp(opcode, "mul") == 0)
  {
    insertZR(argv, argc, 3);
    return multi(argv, argc, 0);
  }
  else if (strcmp(opcode, "mneg") == 0)
  {
    insertZR(argv, argc, 3);
    return multi(argv, argc, 1);
  }
  else if (strcmp(opcode, "b") == 0)
  {
    return branch(argv, argc);
  }
  else if (strcmp(opcode, "br") == 0)
  {
    insertStr(argv, argc, 0, opcode);
    return branch(argv, argc);
  }
  else if (strncmp(opcode, "b.", 2) == 0)
  {
    insertStr(argv, argc, 0, opcode+2);
    return branch(argv, argc);
  }
  else if (strcmp(opcode, "str") == 0)
  {
    return ldstr(argv, argc, 0);
  }
  else if (strcmp(opcode, "ldr") == 0)
  {
    return ldstr(argv, argc, 1);
  }
  
}

void insertZR(char **argv, int argc, int pos)
{
  if (argv[0][0] == 'w')
  {
    insertStr(argv, argc, pos, WZR);
  }
  else if(argv[0][0] == 'x')
  {
    insertStr(argv, argc, pos, XZR);
  }
  else
  {
    printf("Cautious: Invalid register type detected.");
    insertStr(argv, argc, pos, XZR);
  }
}

void insertStr(char **argv, int argc, int pos, char *str)
{
  for (int i = argc - 1; i >= pos; i--)
  {
    argv[i] = argv[i-1];
  }
  argv[pos] = str;
}