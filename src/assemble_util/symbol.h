// organises the symbol table
#ifndef SYMBOL_H
#define SYMBOL_H

#include "../common/datatypes.h"

#define SYMBLOAD_ERROR (-1)

// add a label-address pair into the table
extern int symbadd(char *label, addr_t addr);

// check if a label already exists
extern int8_t existlabel(char *label);

// get the address of a label
extern addr_t symbget(char *label, int len);

// free the symbol table
extern void freesymbtbl();

#endif
