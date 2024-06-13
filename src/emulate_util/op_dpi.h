// data processing (immediate) operation
#ifndef OP_DPI_H
#define OP_DPI_H

#include "datatypes.h"
#include "state.h"
#include "bitwise.h"

#define DPI_OPI_AR      (0b010)
#define DPI_OPI_WM      (0b101)

#define DPI_OPC_ADD     (0b00)
#define DPI_OPC_ADDS    (0b01)
#define DPI_OPC_SUB     (0b10)
#define DPI_OPC_SUBS    (0b11)

#define DPI_OPC_MOVN    (0b00)
#define DPI_OPC_MOVZ    (0b10)
#define DPI_OPC_MOVK    (0b11)

// data processing (immediate)
extern void dpi(instr_t instr);

#endif
