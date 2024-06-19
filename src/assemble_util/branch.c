#include "branch.h"

#include "arg.h"
#include "asmutil.h"
#include "basics.h"
#include "../common/consts.h"

// converts a branch operation to binary
instr_t branch(seg_t *argv, int argc, seg_t cond)
{
    assert(argc == 2);

    if (argv[0] == ARG_T_REGX)
    {
        seg_t xn = argv[1];
        return br(BR_TMPL_REG, DEFAULT_NO_ARG, xn, DEFAULT_NO_ARG, DEFAULT_NO_ARG);
    }
    else
    {
        assert(argv[0] == ARG_T_LIT);
        if (cond == BR_COND_AL)
        {
            seg_t simm26 = argv[1];
            return br(BR_TMPL_UNC, simm26, DEFAULT_NO_ARG, DEFAULT_NO_ARG, DEFAULT_NO_ARG);
        }
        else
        {
            seg_t simm19 = argv[1];
            return br(BR_TMPL_CON, DEFAULT_NO_ARG, DEFAULT_NO_ARG, simm19, cond);
        }
    }

    assert(0);
    return 0;
}
