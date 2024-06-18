#include "arg.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BASE_DEFAULT (0)
#define parseint(ptr, buffer) *buffer = strtol((*ptr), (ptr), BASE_DEFAULT)

static void ltrim(char **chrptr)
{
    for (; **chrptr && isspace(**chrptr); (*chrptr)++)
        ;
}

static void parsereg(char **chrptr, int *buffer)
{
    if (tolower(**chrptr) == 'z' && tolower(*(*chrptr + 1)) == 'r')
    {
        *buffer = 31;
        chrptr += 2;
    }
    else
    {
        parseint(chrptr, buffer);
    }
}

// parse the next argument, and set the char pointer to, if possible, the beginning of next argument
arg_t parsearg(char **str, int *arg1, int *arg2)
{
    printf("parsing: %s\n", *str);
    arg_t retv, argtype;
    ltrim(str);

    if (!**str)
    {
        return ARG_T_NOARG;
    }

    int intbuffer = 0;
    switch (tolower(*(*str)++))
    {
    case 'w':   // 32-bit register
        parsereg(str, &intbuffer);
        assert(intbuffer >= 0 && intbuffer < 32);
        *arg1 = intbuffer;
        retv = ARG_T_REGW;
        break;
    case 'x':   // 64-bit register
        parsereg(str, &intbuffer);
        assert(intbuffer >= 0 && intbuffer < 32);
        *arg1 = intbuffer;
        retv = ARG_T_REGX;
        break;
    case '#':   // immediate value
        parseint(str, &intbuffer);
        assert(intbuffer >= 0 && intbuffer < 32);
        *arg1 = intbuffer;
        retv = ARG_T_IMM;
        break;
    case '[':   // register address
        ltrim(str);
        argtype = parsearg(str, arg1, &intbuffer);
        if (argtype == ARG_T_REGX)
        {
            if (**str != ']')    // address with offset
            {
                ltrim(str);
                argtype = parsearg(str, arg2, &intbuffer);
                if (argtype == ARG_T_REGX)   // register offset
                {
                    assert(**str == ']');
                    (*str)++;
                    retv = ARG_T_ARR;
                }
                else if (argtype == ARG_T_IMM)   // imm offset
                {
                    assert(**str == ']');
                    (*str)++;
                    if (**str == '!')   // pre-index
                    {
                        retv = ARG_T_APRE;
                        (*str)++;
                    }
                    else    // register unchanged
                    {
                        retv = ARG_T_AREG;
                    }
                }
                else    // symtax error at offset
                {
                    assert(0);
                }
            }
            else    // post-index treated as separate argument
            {
                assert(**str == ']');
            }
        }
        else    // immediate address
        {
            assert(**str == ']');
            (*str)++;
            retv = ARG_T_AIMM;
        }
        break;
    default:    // consider rotations
        if (strncmp(*str, "lsl", 3))
        {
            (*str) += 3;
            ltrim(str);
            assert(**str == '#');
            (*str)++;
            parseint(str, &intbuffer);
            retv = ARG_T_LSL;
        }
        else if (strncmp(*str, "lsr", 3))
        {
            (*str) += 3;
            ltrim(str);
            assert(**str == '#');
            (*str)++;
            parseint(str, &intbuffer);
            retv = ARG_T_LSR;
        }
        else if (strncmp(*str, "asr", 3))
        {
            (*str) += 3;
            ltrim(str);
            assert(**str == '#');
            (*str)++;
            parseint(str, &intbuffer);
            retv = ARG_T_ASR;
        }
        else if (strncmp(*str, "ror", 3))
        {
            (*str) += 3;
            ltrim(str);
            assert(**str == '#');
            (*str)++;
            parseint(str, &intbuffer);
            retv = ARG_T_ROR;
        }
        else
        {
            assert(0);
        }
        break;
    }

    if (*(*str) == ',')
    {
        (*str)++;
    }
    ltrim(str);
    return retv;
}
