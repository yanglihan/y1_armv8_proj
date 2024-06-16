#include "interpret.h"

#include "bitwise.h"
#include "op_dpi.h"
#include "op_dpr.h"
#include "op_ls.h"
#include "op_br.h"

// interpret an instruction
int interpret(instr_t instr)
{
    seg_t op0;
    op0 = take_bits(&instr, 25, 4);
    if (C_INSTR_HALT(instr)) // halting
    {
        return 1;
    }
    if (C_OP0_DPI(op0)) // data processing (immediate)
    {
        dpi(instr);
    }
    else if (C_OP0_DPR(op0)) // data processing (register)
    {
        dpr(instr);
    }
    else if (C_OP0_LS(op0)) // loads and stores
    {
        ls(instr);
    }
    else if (C_OP0_BR(op0)) // branches
    {
        br(instr);
    }
    else // unknown command
    {
        return 1;
    }
    return 0;
}
