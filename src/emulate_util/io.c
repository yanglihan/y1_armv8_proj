#include "io.h"

#include <stdio.h>
#include "memory.h"
#include "print_state.h"

// write to terminal, always returns success
int write_terminal(void) {
  print_state();
  return IO_SUCCESS;
}

// write to file, returns failure if output file is invalid
int write_file(char* path)
{
  FILE* output_file = fopen(path, "w");
  if (output_file == NULL) // invalid output file
  {
    print_state();
    return IO_FAILURE;
  }
  fprint_state(output_file);
  fclose(output_file);
  return IO_SUCCESS;
}

// load a program to memory, returns failure if input file is invalid
int load_program(char* path)
{
  FILE* input_file = fopen(path, "rb");
  if (input_file == NULL) // invalid source file
  {
    return IO_FAILURE;
  }
  load_mem(input_file);
  fclose(input_file);
  return IO_SUCCESS;
}
