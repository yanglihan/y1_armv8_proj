#include "interpret.h"

#include "bitwise.h"
#include "operations.h"

// interpret an instruction
int interpret(instr_t instr)
{
    seg_t op0;
    op0 = bitfield(&instr, 25, 4);
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
