#include "ldstr.h"

#include "arg.h"
#include "asmutil.h"
#include "basics.h"
#include "../common/consts.h"

// converts a load/store operation to binary
instr_t ldstr(seg_t *argv, int argc, seg_t load)
{
    assert(argc == 4 || argc == 5 || argc == 7);
    seg_t sf = argv[0] == ARG_T_REGX;
    seg_t rt = argv[1];

    if (argv[2] == ARG_T_AIMM) // immdediate address
    {
        assert(argc == 4);
        seg_t offset = argv[3];
        return ll(sf, offset, rt);
    }
    else if (argv[2] == ARG_T_APRE) // pre-index
    {
        seg_t xn = argv[3];
        seg_t offset = SDT_OFFS_TMPL_PRE | (trun64(argv[4], 9) << 2);
        return sdt(sf, SDT_U_SIGNED, load, offset, xn, rt);
    }
    else if (argv[2] == ARG_T_ARR) // register offset
    {
        seg_t xn = argv[3];
        seg_t offset = SDT_OFFS_TMPL_REG | (argv[4] << 6);
        return sdt(sf, SDT_U_SIGNED, load, offset, xn, rt);
    }
    else if (argv[2] == ARG_T_AREG) // register address
    {
        seg_t xn = argv[3];
        if (argc == 5) // unsigned offset
        {
            seg_t offset = SDT_OFFS_TMPL_IMM | utrun64(argv[4] >> (sf ? 3 : 2), 12);
            return sdt(sf, SDT_U_UNSIGNED, load, offset, xn, rt);
        }
        else // post index
        {
            assert(argc == 7);
            assert(argv[5] == ARG_T_IMM);
            seg_t offset = SDT_OFFS_TMPL_POS | (trun64(argv[6], 9) << 2);
            return sdt(sf, SDT_U_SIGNED, load, offset, xn, rt);
        }
    }
    else if (argv[2] == ARG_T_LIT) // literal address
    {
        assert(load);
        seg_t addr = trun64(argv[3], 19);
        return ll(sf, addr, rt);
    }
    
    assert(0);
    return 0;
}
