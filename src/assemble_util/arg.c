#include "arg.h"

#include "asmutil.h"
#include "asmconsts.h"
#include "symbol.h"

#define BASE_DEFAULT (0)

// parse an integer regardless of base (10 or 16)
void parseint(char **chrptr, int *buffer)
{
    *buffer = strtol((*chrptr), (chrptr), BASE_DEFAULT);
}

// parse the index of a register
void parsereg(char **chrptr, int *buffer)
{

    if (strncasecmp(*chrptr, "zr", 2))
    {
        *buffer = ZR_INDEX;
        chrptr += 2;
    }
    else
    {
        parseint(chrptr, buffer);
    }
}

// parse the next argument, and set the char pointer to, if possible, the beginning of next argument
static arg_t parsearg(char **str, int *arg1, int *arg2)
{
    arg_t retv, argtype;
    ltrim(str);

    if (!**str)
    {
        return ARG_T_NOARG;
    }

    int intbuffer = 0;
    switch (tolower(*(*str)++))
    {
    case 'w': // 32-bit register
        parsereg(str, &intbuffer);
        assert(intbuffer >= 0 && intbuffer < 32);
        *arg1 = intbuffer;
        retv = ARG_T_REGW;
        break;
    case 'x': // 64-bit register
        parsereg(str, &intbuffer);
        assert(intbuffer >= 0 && intbuffer < 32);
        *arg1 = intbuffer;
        retv = ARG_T_REGX;
        break;
    case '#': // immediate value
        parseint(str, &intbuffer);
        assert(intbuffer >= 0 && intbuffer < 32);
        *arg1 = intbuffer;
        retv = ARG_T_IMM;
        break;
    case '[': // register address
        ltrim(str);
        argtype = parsearg(str, arg1, &intbuffer);
        if (argtype == ARG_T_REGX)
        {
            if (**str != ']') // address with offset
            {
                ltrim(str);
                argtype = parsearg(str, arg2, &intbuffer);
                if (argtype == ARG_T_REGX) // register offset
                {
                    assert(**str == ']');
                    (*str)++;
                    retv = ARG_T_ARR;
                }
                else if (argtype == ARG_T_IMM) // imm offset
                {
                    assert(**str == ']');
                    (*str)++;
                    if (**str == '!') // pre-index
                    {
                        retv = ARG_T_APRE;
                        (*str)++;
                    }
                    else // register unchanged
                    {
                        retv = ARG_T_AREG;
                    }
                }
                else // symtax error at offset
                {
                    assert(0);
                }
            }
            else // post-index treated as separate argument
            {
                ltrim(str);
                assert(**str == ']');
                retv = ARG_T_AREG;
                *arg2 = 0;
                (*str)++;
            }
        }
        else // immediate address
        {
            assert(**str == ']');
            (*str)++;
            retv = ARG_T_AIMM;
        }
        break;
    default: // consider rotations
        if (strncmp(*str, "lsl", 3))
        {
            (*str) += 3;
            ltrim(str);
            assert(**str == '#');
            (*str)++;
            parseint(str, arg1);
            retv = ARG_T_LSL;
        }
        else if (strncmp(*str, "lsr", 3))
        {
            (*str) += 3;
            ltrim(str);
            assert(**str == '#');
            (*str)++;
            parseint(str, arg1);
            *arg1 = intbuffer;
            retv = ARG_T_LSR;
        }
        else if (strncmp(*str, "asr", 3))
        {
            (*str) += 3;
            ltrim(str);
            assert(**str == '#');
            (*str)++;
            parseint(str, arg1);
            retv = ARG_T_ASR;
        }
        else if (strncmp(*str, "ror", 3))
        {
            (*str) += 3;
            ltrim(str);
            assert(**str == '#');
            (*str)++;
            parseint(str, arg1);
            retv = ARG_T_ROR;
        }
        else // assume literal
        {
            retv = ARG_T_LIT;
            char *begin = *str;
            while (!isspace(**str) && (**str != ',') && (**str != '\0'))
            {
                (*str)++;
            }
            *arg1 = symbget(begin, *str - begin);
            ltrim(str);
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

// parse all arguments in a given string, returns the number of arguments
int parseall(char *line, int *argv)
{
    char *chrptr = line;
    int buffer1, buffer2;
    int argc = 0;
    arg_t argtype;

    while (1)
    {
        argtype = parsearg(&chrptr, &buffer1, &buffer2);
        if (argtype == ARG_T_NOARG)
        {
            break;
        }
        if (argtype == ARG_T_AREG || argtype == ARG_T_APRE || argtype == ARG_T_ARR)
        {
            argv[argc++] = argtype;
            argv[argc++] = buffer1;
            argv[argc++] = buffer2;
        }
        else
        {
            argv[argc++] = argtype;
            argv[argc++] = buffer1;
        }
    }
    return argc;
}
