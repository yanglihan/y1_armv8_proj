// defines a package storing
#ifndef ARG_H
#define ARG_H

#include "../common/datatypes.h"

// flags for integer arguments
typedef enum arg
{
    ARG_T_NOARG = 0,
    ARG_T_IMM,
    ARG_T_REGW,
    ARG_T_REGX,
    ARG_T_AIMM,
    ARG_T_AREG,
    ARG_T_ARR,
    ARG_T_APRE,
    ARG_T_LSL,
    ARG_T_LSR,
    ARG_T_ASR,
    ARG_T_ROR,
    ARG_T_LIT,
} arg_t;

// parse an integer regardless of base (10 or 16)
extern void parseint(char **chrptr, seg_t *buffer);

// parse the index of a register
void parsereg(char **chrptr, seg_t *buffer);

// parse the next argument, and set the char pointer to, if possible, the beginning of next argument
extern arg_t parsearg(char **str, seg_t *arg1, seg_t *arg2, int pos);

// parse all arguments in a given string, returns the number of arguments
extern int parseall(char *line, seg_t *argv, int pos);

#endif
