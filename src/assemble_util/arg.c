#include "arg.h"

#include "asmconsts.h"
#include "asmutil.h"
#include "symbol.h"

#define BASE_DEFAULT (0)

// parse an integer regardless of base (10 or 16)
void parseint(char **chrptr, seg_t *buffer)
{
    *buffer = strtoul((*chrptr), (chrptr), BASE_DEFAULT);
}

// parse the index of a register
void parsereg(char **chrptr, seg_t *buffer)
{
    if (!strncasecmp(*chrptr, "zr", 2)) // zero register
    {
        *buffer = ZR_INDEX;
        (*chrptr) += 2;
    }
    else // general registers
    {
        parseint(chrptr, buffer);
    }
}

// parse the next argument, and set the char pointer to, if possible, the beginning of next argument
arg_t parsearg(char **str, seg_t *arg1, seg_t *arg2, int pos)
{
    arg_t retv, argtype;
    ltrim(str);

    if (!**str) // empty string
    {
        return ARG_T_NOARG;
    }

    seg_t intbuffer = 0;
    switch (tolower(**str))
    {
    case 'w': // 32-bit register
        if (((strncasecmp(*str, "wzr", 3) && !sscanf(*str, "w%lld", &intbuffer))) || !(isspace(*(*str + 3)) || (*(*str + 3) == ',' || !*(*str + 3))))
        // not register format
        {
            retv = ARG_T_LIT;
            break;
        }
        (*str)++;
        parsereg(str, &intbuffer);
        assert(intbuffer >= 0 && intbuffer < 32);
        *arg1 = intbuffer;
        retv = ARG_T_REGW;
        break;
    case 'x': // 64-bit register
        if (((strncasecmp(*str, "xzr", 3) &&!sscanf(*str, "x%lld", &intbuffer)) || !(isspace(*(*str + 3)) || (*(*str + 3) == ',' || !*(*str + 3)))))
        // not register format
        {
            retv = ARG_T_LIT;
            break;
        }
        (*str)++;
        parsereg(str, &intbuffer);
        assert(intbuffer >= 0 && intbuffer < 32);
        *arg1 = intbuffer;
        retv = ARG_T_REGX;
        break;
    case '#': // immediate value
        (*str)++;
        parseint(str, &intbuffer);
        *arg1 = intbuffer;
        retv = ARG_T_IMM;
        break;
    case '[': // register address
        (*str)++;
        ltrim(str);
        argtype = parsearg(str, arg1, &intbuffer, pos);
        if (argtype == ARG_T_REGX)
        {
            if (**str != ']') // address with offset
            {
                ltrim(str);
                argtype = parsearg(str, arg2, &intbuffer, pos);
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
    default: // consider shifts
        if (!strncmp(*str, "lsl", 3))
        {
            (*str) += 3;
            ltrim(str);
            assert(**str == '#');
            (*str)++;
            parseint(str, arg1);
            retv = ARG_T_LSL;
        }
        else if (!strncmp(*str, "lsr", 3))
        {
            (*str) += 3;
            ltrim(str);
            assert(**str == '#');
            (*str)++;
            parseint(str, arg1);
            retv = ARG_T_LSR;
        }
        else if (!strncmp(*str, "asr", 3))
        {
            (*str) += 3;
            ltrim(str);
            assert(**str == '#');
            (*str)++;
            parseint(str, arg1);
            retv = ARG_T_ASR;
        }
        else if (!strncmp(*str, "ror", 3))
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
        }
        break;
    }
    if (retv == ARG_T_LIT) // consider literal
    {
        char *begin = *str;
        while (!isspace(**str) && (**str != ',') && (**str != '\0'))
        {
            (*str)++;
        }
        *arg1 = (symbget(begin, *str - begin) - pos);
        ltrim(str);
    }

    if (*(*str) == ',')
    {
        (*str)++;
    }
    ltrim(str);
    return retv;
}

// parse all arguments in a given string, returns the number of arguments
int parseall(char *line, seg_t *argv, int pos)
{
    char *chrptr = line;
    seg_t buffer1, buffer2;
    int argc = 0;
    arg_t argtype;

    while (1)
    {
        argtype = parsearg(&chrptr, &buffer1, &buffer2, pos);
        if (argtype == ARG_T_NOARG) // empty
        {
            break;
        }
        if (argtype == ARG_T_AREG || argtype == ARG_T_APRE || argtype == ARG_T_ARR) // double-value
        {
            argv[argc++] = argtype;
            argv[argc++] = buffer1;
            argv[argc++] = buffer2;
        }
        else // single-value
        {
            argv[argc++] = argtype;
            argv[argc++] = buffer1;
        }
    }
    return argc;
}
