#include "asmline.h"

#include "arg.h"
#include "asmconsts.h"
#include "asmutil.h"
#include "symbol.h"
#include "operations.h"
#include "../common/consts.h"

// reads the mnemonic of a condition
seg_t mnecond(char *mne)
{
    if (!strcasecmp(mne, "eq"))
    {
        return BR_COND_EQ;
    }
    else if (!strcasecmp(mne, "ne"))
    {
        return BR_COND_NE;
    }
    else if (!strcasecmp(mne, "ge"))
    {
        return BR_COND_GE;
    }
    else if (!strcasecmp(mne, "lt"))
    {
        return BR_COND_LT;
    }
    else if (!strcasecmp(mne, "gt"))
    {
        return BR_COND_GT;
    }
    else if (!strcasecmp(mne, "le"))
    {
        return BR_COND_LE;
    }
    else if (!strcasecmp(mne, "al"))
    {
        return BR_COND_AL;
    }
    return 0;
}

// reads the mnemonic of an instruction
instr_t mneread(char *mne, seg_t *argv, int argc)
{
    if (!strcasecmp(mne, "add"))
    {

        return arith(argv, argc, ARITH_OPC_ADD);
    }
    else if (!strcasecmp(mne, "adds"))
    {
        return arith(argv, argc, ARITH_OPC_ADDS);
    }
    else if (!strcasecmp(mne, "sub"))
    {
        return arith(argv, argc, ARITH_OPC_SUB);
    }
    else if (!strcasecmp(mne, "subs"))
    {
        return arith(argv, argc, ARITH_OPC_SUBS);
    }
    else if (!strcasecmp(mne, "cmp"))
    {
        argc = insnelem(argv, argc, 0, argv, 2); // first argument should always be regw or regx
        argv[1] = ZR_INDEX;
        return arith(argv, argc, ARITH_OPC_SUBS);
    }
    else if (!strcasecmp(mne, "cmn"))
    {
        argc = insnelem(argv, argc, 0, argv, 2); // first argument should always be regw or regx
        argv[1] = ZR_INDEX;
        return arith(argv, argc, ARITH_OPC_ADDS);
    }
    else if (!strcasecmp(mne, "neg"))
    {
        argc = insnelem(argv, argc, 2, argv, 2); // first argument should always be regw or regx
        argv[3] = ZR_INDEX;
        return arith(argv, argc, ARITH_OPC_SUB);
    }
    else if (!strcasecmp(mne, "negs"))
    {
        argc = insnelem(argv, argc, 2, argv, 2); // first argument should always be regw or regx
        argv[3] = ZR_INDEX;
        return arith(argv, argc, ARITH_OPC_SUBS);
    }
    else if (!strcasecmp(mne, "and"))
    {
        return logic(argv, argc, DPR_OPC_AND, LOGIC_NONEG);
    }
    else if (!strcasecmp(mne, "ands"))
    {
        return logic(argv, argc, DPR_OPC_ANDS, LOGIC_NONEG);
    }
    else if (!strcasecmp(mne, "bic"))
    {
        return logic(argv, argc, DPR_OPC_AND, LOGIC_NEG);
    }
    else if (!strcasecmp(mne, "bics"))
    {
        return logic(argv, argc, DPR_OPC_ANDS, LOGIC_NEG);
    }
    else if (!strcasecmp(mne, "eor"))
    {
        return logic(argv, argc, DPR_OPC_EOR, LOGIC_NONEG);
    }
    else if (!strcasecmp(mne, "eon"))
    {
        return logic(argv, argc, DPR_OPC_EOR, LOGIC_NEG);
    }
    else if (!strcasecmp(mne, "orr"))
    {
        return logic(argv, argc, DPR_OPC_ORR, LOGIC_NONEG);
    }
    else if (!strcasecmp(mne, "orn"))
    {
        return logic(argv, argc, DPR_OPC_ORR, LOGIC_NEG);
    }
    else if (!strcasecmp(mne, "tst"))
    {
        argc = insnelem(argv, argc, 0, argv, 2); // first argument should always be regw or regx
        argv[1] = ZR_INDEX;
        return logic(argv, argc, DPR_OPC_ANDS, LOGIC_NONEG);
    }
    else if (!strcasecmp(mne, "mvn"))
    {
        argc = insnelem(argv, argc, 2, argv, 2); // first argument should always be regw or regx
        argv[3] = ZR_INDEX;
        return logic(argv, argc, DPR_OPC_ORR, LOGIC_NEG);
    }
    else if (!strcasecmp(mne, "mov"))
    {
        argc = insnelem(argv, argc, 2, argv, 2); // first argument should always be regw or regx
        argv[3] = ZR_INDEX;
        return logic(argv, argc, DPR_OPC_ORR, LOGIC_NONEG);
    }
    else if (!strcasecmp(mne, "movn"))
    {
        return move(argv, argc, DPI_OPC_MOVN);
    }
    else if (!strcasecmp(mne, "movk"))
    {
        return move(argv, argc, DPI_OPC_MOVK);
    }
    else if (!strcasecmp(mne, "movz"))
    {
        return move(argv, argc, DPI_OPC_MOVZ);
    }
    else if (!strcasecmp(mne, "madd"))
    {
        return multi(argv, argc, ARITH_NONEG);
    }
    else if (!strcasecmp(mne, "msub"))
    {
        return multi(argv, argc, ARITH_NEG);
    }
    else if (!strcasecmp(mne, "mul"))
    {
        argc = insnelem(argv, argc, 6, argv, 2); // first argument should always be regw or regx
        argv[7] = ZR_INDEX;
        return multi(argv, argc, ARITH_NONEG);
    }
    else if (!strcasecmp(mne, "mneg"))
    {
        argc = insnelem(argv, argc, 6, argv, 2); // first argument should always be regw or regx
        argv[7] = ZR_INDEX;
        return multi(argv, argc, ARITH_NEG);
    }
    else if (!strcasecmp(mne, "b"))
    {
        return branch(argv, argc, BR_COND_AL);
    }
    else if (!strcasecmp(mne, "br"))
    {
        return branch(argv, argc, BR_COND_AL);
    }
    else if (!strncasecmp(mne, "b.", 2))
    {
        return branch(argv, argc, mnecond(mne + 2));
    }
    else if (!strcasecmp(mne, "str"))
    {
        return ldstr(argv, argc, LDSTR_STR);
    }
    else if (!strcasecmp(mne, "ldr"))
    {
        return ldstr(argv, argc, LDSTR_LD);
    }
    return 0x0;
}

// converts a single line to binary, returns offset
int asmline(char *line, instr_t *buffer)
{
    char *ptr = line;
    char *mne;
    seg_t argv[40];
    int argc;
    ltrim(&ptr);

    if (*ptr == '\0') // empty line
    {
        return 0;
    }

    if (*(ptr + strlen(ptr) - 1) == ':')
    {
        return 0;
    }
    
    if (*ptr == '.')
    {
        if (!strncasecmp(ptr, ".int", 4))
        {
            ptr += 4;
            ltrim(&ptr);
            parseint(&ptr, buffer);
            return 1;
        }
        if (!strncasecmp(ptr, ".string", 7))
        {
            int offset;
            ptr += 7;
            ltrim(&ptr);
            assert(*ptr == '"');
            offset = cpyunesc(ptr + 1, (void *)buffer);
            offset = (offset + 3) >> 2;
            return offset;
        }
        return 0;
    }

    mne = strsep(&line, " \t");
    argc = parseall(line, argv);

    instr_t bin = mneread(mne, argv, argc);
    *buffer = bin;
    return bin ? 1 : 0;
}

// converts a single line of directive to binary and adds label to symbol table
// treats instrucitions as blank, returns offset
int preasmline(char *line, instr_t *buffer, int pos)
{
    char *ptr = line;
    char *mne;
    seg_t argv[40];
    int argc;
    ltrim(&ptr);

    if (!*ptr) // empty line
    {
        return 0;
    }

    if (*(ptr + strlen(ptr) - 1) == ':') // label
    {
        printf("preasmline: label %s encountered\n", ptr); // debug
        assert(strchr(ptr, ' ') == NULL);
        assert(strchr(ptr, '\t') == NULL);
        assert(isalpha(*ptr));
        symbadd(ptr, pos << 2);
        return 0;
    }

    if (*ptr == '.')
    {
        if (!strncasecmp(ptr, ".int", 4))
        {
            ptr += 4;
            ltrim(&ptr);
            parseint(&ptr, buffer);
            return 1;
        }
        if (!strncasecmp(ptr, ".string", 7))
        {
            int offset;
            ptr += 7;
            ltrim(&ptr);
            assert(*ptr == '"');
            offset = cpyunesc(ptr + 1, (void *)buffer);
            offset = (offset + 3) >> 2;
            return offset;
        }
        return 0;
    }
    
    // instructions are ignored
    return 1;
}
