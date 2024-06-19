#include "basics.h"
#include "stdio.h" // debug

// data processing (immediate)
instr_t dpi(seg_t sf, seg_t opc, seg_t opi, seg_t operand, seg_t rd)
{
    printf("dpi sf %#llx, opc %#llx, opi %#llx, oper %#llx, rd %#llx\n",
           sf, opc, opi, operand, rd); // debug
    printf("dpi result -- %#llx\n",
           (sf << 31) | (opc << 29) | (0b100 << 26) | (opi << 23) | (operand << 5) | rd); // debug
    return (sf << 31) | (opc << 29) | (0b100 << 26) | (opi << 23) | (operand << 5) | rd;
}

// data processing (register)
instr_t dpr(seg_t sf, seg_t opc, seg_t m, seg_t opr, seg_t rm, seg_t operand, seg_t rn, seg_t rd)
{
    printf("dpr sf %#llx, opc %#llx, m %#llx, opr %#llx, rm %#llx, oper %#llx, rn %#llx, rd %#llx\n",
           sf, opc, m, opr, rm, operand, rn, rd); // debug
    return (sf << 31) | (opc << 29) | (m << 28) | (0b101 << 25) | (opr << 21) | (rm << 16) | (operand << 10) | (rn << 5) | rd;
}

// single data transfer
instr_t sdt(seg_t sf, seg_t l, seg_t offset, seg_t xn, seg_t rt)
{
    printf("sdt sf %#llx, l %#llx, offset %#llx, xn %#llx, rt %#llx\n",
           sf, l, offset, xn, rt); // debug
    return (1 << 31) | (sf << 30) | (0b1110010 << 23) | (l << 22) | (offset << 10) | (xn << 5) | rt;
}

// load literal
instr_t ll(seg_t sf, seg_t simm19, seg_t rt)
{
    printf("ll sf %#llx, simm19 %#llx, rt %#llx\n",
           sf, simm19, rt); // debug
    return (0 << 31) | (sf << 30) | (0b011000 << 24) | (simm19 << 5) | rt;
}

// branch
instr_t br(seg_t tmpl, seg_t simm26, seg_t xn, seg_t simm19, seg_t cond)
{
    printf("br tmpl %#llx, simm26 %#llx, xn %#llx, simm19 %#llx, cond %#llx\n",
           tmpl, simm26, xn, simm19, cond); // debug
    return tmpl | simm26 | (xn << 5) | (simm19 << 5) | cond;
}
