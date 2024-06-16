#include "basics.h"

// data processing (immediate)
instr_t dpi(seg_t sf, seg_t opc, seg_t opi, seg_t operand, seg_t rd)
{
    return (sf << 31) | (opc << 29) | (0b100 << 26) | (opi << 23) | (operand << 5) | rd;
}

// data processing (register)
instr_t dpr(seg_t sf, seg_t opc, seg_t m, seg_t opr, seg_t rm, seg_t operand, seg_t rn, seg_t rd)
{
    return (sf << 31) | (opc << 29) | (m << 28) | (0b101 << 25) | (opr << 21) | (rm << 16) | (operand << 10) | (rn << 5) | rd;
}

// single data transfer
instr_t sdt(seg_t sf, seg_t l, seg_t offset, seg_t xn, seg_t rt)
{
    return (1 << 31) | (sf << 30) | (0b1110010 << 23) | (l << 22) | (offset << 10) | (xn << 5) | rt;
}

// load literal
instr_t ll(seg_t sf, seg_t simm19, seg_t rt)
{
    return (0 << 31) | (sf << 30) | (0b011000 << 24) | (simm19 << 5) | rt;
}

// branch
instr_t br(seg_t operand)
{
    return (0b0101 << 26) | operand;
}
