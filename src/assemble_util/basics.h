// basic instructions
#ifndef BASICS_H
#define BASICS_H

#include "../common/datatypes.h"

#include "basics.h"

// data processing (immediate)
extern instr_t dpi(seg_t sf, seg_t opc, seg_t opi, seg_t operand, seg_t rd);

// data processing (register)
extern instr_t dpr(seg_t sf, seg_t opc, seg_t m, seg_t opr, seg_t rm, seg_t operand, seg_t rn, seg_t rd);

// single data transfer
extern instr_t sdt(seg_t sf, seg_t l, seg_t offset, seg_t xn, seg_t rt);

// load literal
extern instr_t ll(seg_t sf, seg_t simm19, seg_t rt);

// branch
extern instr_t br(seg_t operand);

#endif
