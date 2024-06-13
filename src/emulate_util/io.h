// sealed IO operations
#ifndef IO_H
#define IO_H

#include <stdio.h>

#include "print_state.h"

#define IO_SUCCESS (0)
#define IO_FAILURE (1)

// write to terminal, always returns success
int write_terminal(void);

// write to file, returns failure if output file is invalid
int write_file(char* path);

// load a program to memory, returns failure if input file is invalid
int load_program(char* path);

#endif
