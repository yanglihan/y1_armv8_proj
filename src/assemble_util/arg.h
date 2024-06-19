// defines a package storing
#ifndef ARG_H
#define ARG_H

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
void parseint(char **chrptr, int *buffer);

// parse the index of a register
void parsereg(char **chrptr, int *buffer);

// parse all arguments in a given string, returns the number of arguments
extern int parseall(char *line, int *argv);

#endif
