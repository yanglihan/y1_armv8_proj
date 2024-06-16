#include "emulate_util/interpret.h"
#include "emulate_util/io.h"
#include "emulate_util/memory.h"
#include "emulate_util/state.h"
#include "common/datatypes.h"
#include <stdio.h>
#include <stdlib.h>

// #define LINE_BY_LINE_PRINT_DEBUG // to print the resulting state after each instruction (warning: affects performance)
// #define DEBUG                    // for general debugging

#ifdef LINE_BY_LINE_PRINT_DEBUG
#define debug_print_state() print_state()
#else
#define debug_print_state()
#endif

#ifdef DEBUG
#define debug_printf(fstr, ...) printf(fstr, __VA_ARGS__)
#else
#define debug_printf(fstr, ...)
#endif

int main(int argc, char** argv)
{
  if (argc == 2) // terminal output
  {
    debug_printf("Input file: %s\n", argv[1]);
  }
  else if (argc == 3) // file output
  {
    debug_printf("Input file: %s\n", argv[1]);
    debug_printf("Output file: %s\n", argv[2]);
  }
  else
  {
    printf("Invalid number of arguments!\nExpected: %s input_file [output_file]\n", argv[0]);
    return EXIT_SUCCESS;
  }

  if (fload(argv[1])) // load program
  {
    printf("Invalid input file!\n");
    return EXIT_SUCCESS;
  }

  while (1)
  {
    instr_t instr = CUR_INSTR;
    if (interpret(instr)) // interpret returns 1 on halt or error
    {
      printf("Invalid command at location %#08x: %#08x. Program aborted.\n", PC, instr);
      break;
    }
    debug_print_state();
  }

  if (argc == 3) // file output
  {
    if (fout(argv[2])) {
      printf("Invalid output file! The program will now use terminal output.\n");
      tout();
    }
  }
  else // terminal output
  {
    tout();
  }

  return EXIT_SUCCESS;
}
