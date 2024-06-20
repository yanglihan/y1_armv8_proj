
#include "assemble_util/io.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
  char infile[100];
  char outfile[100];
  if (argc != 3)
  {
    printf("Please enter input and output below:\n");
    scanf("%s", infile);
    scanf("%s", outfile);
  }
  else
  {
    strcpy(infile, argv[1]);
    strcpy(outfile, argv[2]);
  }

  if (fpreasm(infile, outfile) >= 0)
  {
    fasm(infile, outfile);
  }
  else
  {
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
