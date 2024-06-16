// for printing current state to file / terminal
#ifndef PRINT_STATE_H
#define PRINT_STATE_H

#include <stdio.h>

// print all registers and non-zero memories
extern void print_state(void);

// print all registers and non-zero memories
extern void fprint_state(FILE* out);

#endif
