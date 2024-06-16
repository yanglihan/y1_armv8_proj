// data processing (register) operation
#ifndef OP_DPR_H
#define OP_DPR_H

#include "../common/datatypes.h"

#define C_DPR_OPR_MULT(opr) (0b1000)
#define C_DPR_OPR_AR(opr)   ((opr & 0b1001) == 0b1000)
#define C_DPR_OPR_BL(opr)   ((opr & 0b1000) == 0b0000)

#define DPR_OPC_ADD     (0b00)
#define DPR_OPC_ADDS    (0b01)
#define DPR_OPC_SUB     (0b10)
#define DPR_OPC_SUBS    (0b11)

#define DPR_OPC_AND     (0b00)
#define DPR_OPC_ORR     (0b01)
#define DPR_OPC_EOR     (0b10)
#define DPR_OPC_ANDS    (0b11)

// data processing (register)
extern void dpr(instr_t instr);

#endif
