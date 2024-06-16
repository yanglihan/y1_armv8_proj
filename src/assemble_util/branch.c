#include "branch.h"

#include "stdutil.h"
#include "../common/consts.h"

// get condition code
static seg_t cond_code(const char *cond)
{
    if (strcmp(cond, "eq") == 0)
    {
        return BR_COND_EQ;
    }
    if (strcmp(cond, "ne") == 0)
    {
        return BR_COND_NE;
    }
    if (strcmp(cond, "ge") == 0)
    {
        return BR_COND_GE;
    }
    if (strcmp(cond, "lt") == 0)
    {
        return BR_COND_LT;
    }
    if (strcmp(cond, "gt") == 0)
    {
        return BR_COND_GT;
    }
    if (strcmp(cond, "le") == 0)
    {
        return BR_COND_LE;
    }
    if (strcmp(cond, "al") == 0)
    {
        return BR_COND_AL;
    }
    assert(0);
    return 0;
}

// converts a branch operation to binary
instr_t branch(char **args, int argc)
{
    assert(argc == 1 || argc == 2);

    if (argc == 1) // b <literal>
    {
        char *label = args[0];
        int offset = calculate_offset(label);
        return br(offset);
    }
    else if (argc == 2)
    {
        if (strcmp(args[0], "br") == 0) // br <Xn>
        {
            char *ptr;
            seg_t xn = strtol(&args[1][1], &ptr, 10);
            return br(xn);
        }
        else // b.cond <literal>
        {
            char *cond = args[0];
            char *label = args[1];
            int offset = calculate_offset(label);
            seg_t condition = cond_code(cond);
            return br_cond(condition, offset);
        }
    }

    assert(0);
    return 0;
}
