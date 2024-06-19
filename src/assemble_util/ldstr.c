#include "ldstr.h"

#include "../common/consts.h"
#include "arg.h"
#include "asmutil.h"
#include "basics.h"

// converts a load/store operation to binary
instr_t ldstr(int *argv, int argc, seg_t load)
{
    assert(argc == 4 || argc == 5 || argc == 6);
    seg_t sf = argv[0] == ARG_T_REGX;
    seg_t rt = argv[1];

    if (argv[2] == ARG_T_AIMM)
    {
        assert(argc == 4);
        seg_t offset = argv[3];
        return ll(sf, offset, rt);
    }
    else if (argv[2] == ARG_T_APRE)
    {
        seg_t xn = argv[3];
        seg_t offset = SDT_OFFS_TMPL_PRE | (argv[4] << 2);
        return sdt(sf, load, offset, xn, rt);
    }
    else if (argv[2] == ARG_T_ARR)
    {
        seg_t xn = argv[3];
        seg_t offset = SDT_OFFS_TMPL_REG | (argv[4] << 6);
        return sdt(sf, load, offset, xn, rt);
    }
    else if (argv[2] == ARG_T_AREG)
    {
        seg_t xn = argv[3];
        if (argc == 4)
        {
            seg_t offset = SDT_OFFS_TMPL_IMM;
            return sdt(sf, load, offset, xn, rt);
        }
        else
        {
            assert(argc == 6);
            assert(argv[4] == ARG_T_IMM);
            seg_t offset = SDT_OFFS_TMPL_POS | (argv[5] << 2);
            return sdt(sf, load, offset, xn, rt);
        }
    }
    else if (argv[2] == ARG_T_LIT)
    {
        assert(load);
        seg_t addr = argv[3];
        return ll(sf, addr, rt);
    }
    
    assert(0);
    return 0;
}
