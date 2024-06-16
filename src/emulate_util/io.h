// sealed IO operations
#ifndef IO_H
#define IO_H

#define IO_SUCCESS (0)
#define IO_FAILURE (1)

// write to terminal, always returns success
int tout(void);

// write to file, returns failure if output file is invalid
int fout(char* path);

// load a program to memory, returns failure if input file is invalid
int fload(char* path);

#endif
