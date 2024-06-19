// sealed IO operations
#ifndef IO_H
#define IO_H

#define IO_FAILURE (-1)

// assembles from in, outputs to out
extern int fasm(const char *infile, const char *outfile);

// pre-assembles from in, outputs to out
extern int fpreasm(const char *infile, const char *outfile);

#endif
