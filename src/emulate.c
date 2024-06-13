
#include "emulate.h"

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
    printf("Invalid number of arguments!\nExpected: emulate input_file [output_file]\n");
    return EXIT_SUCCESS;
  }

  if (load_program(argv[1])) // load program
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
    if (write_file(argv[2])) {
      printf("Invalid output file! The program will now use terminal output.\n");
      write_terminal();
    }
  }
  else // terminal output
  {
    write_terminal();
  }

  return EXIT_SUCCESS;
}
