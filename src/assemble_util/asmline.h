// assemble a single line of instruction
#ifndef ASMLINE_H
#define ASMLINE_H

#include "../common/datatypes.h"

#define ASM_LINE_SKIP (-1)

// converts a single line to binary, returns offset
extern int asmline(char *line, instr_t *buffer, int pos);

// converts a single line of directive to binary and adds label to symbol table
// treats instrucitions as blank, returns offset
extern int preasmline(char *line, instr_t *buffer, int pos);

#endif
