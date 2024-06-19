#include "arg.h"

#include "asmconsts.h"
#include "asmutil.h"
#include "symbol.h"

#define BASE_DEFAULT (0)

// parse an integer regardless of base (10 or 16)
void parseint(char **chrptr, seg_t *buffer)
{
    printf("parseint: chrptr => %s\n", *chrptr); // debug
    *buffer = strtoul((*chrptr), (chrptr), BASE_DEFAULT);
}

// parse the index of a register
void parsereg(char **chrptr, seg_t *buffer)
{
    printf("parsereg: chrptr => %s\n", *chrptr); // debug
    if (!strncasecmp(*chrptr, "zr", 2))
    {
        *buffer = ZR_INDEX;
        (*chrptr) += 2;
    }
    else
    {
        parseint(chrptr, buffer);
    }
}

// parse the next argument, and set the char pointer to, if possible, the beginning of next argument
static arg_t parsearg(char **str, seg_t *arg1, seg_t *arg2)
{
    arg_t retv, argtype;
    ltrim(str);
    printf("parsearg: str => %s\n", *str); // debug

    if (!**str)
    {
        return ARG_T_NOARG;
    }

    seg_t intbuffer = 0;
    switch (tolower(**str))
    {
    case 'w':                                     // 32-bit register
        (*str)++;
        printf("parsearg: case w => %s\n", *str); // debug
        parsereg(str, &intbuffer);
        assert(intbuffer >= 0 && intbuffer < 32);
        *arg1 = intbuffer;
        retv = ARG_T_REGW;
        break;
    case 'x':                                     // 64-bit register
        (*str)++;
        printf("parsearg: case x => %s\n", *str); // debug
        parsereg(str, &intbuffer);
        assert(intbuffer >= 0 && intbuffer < 32);
        *arg1 = intbuffer;
        retv = ARG_T_REGX;
        break;
    case '#':                                     // immediate value
        (*str)++;
        printf("parsearg: case # => %s\n", *str); // debug
        parseint(str, &intbuffer);
        *arg1 = intbuffer;
        retv = ARG_T_IMM;
        break;
    case '[':                                     // register address
        (*str)++;
        printf("parsearg: case [ => %s\n", *str); // debug
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
    default:                                                      // consider shifts
        printf("parsearg: consider shifts str => %s\n", *str);    // debug
        printf("parsearg: lsl => %d\n", strncmp(*str, "lsl", 3)); // debug
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
            char *begin = *str;
            while (!isspace(**str) && (**str != ',') && (**str != '\0'))
            {
                (*str)++;
            }
            *arg1 = symbget(begin, *str - begin);
            printf("parsearg: successfully found literal %s at %lu\n", begin, *arg1); // debug
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
int parseall(char *line, seg_t *argv)
{
    char *chrptr = line;
    seg_t buffer1, buffer2;
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
