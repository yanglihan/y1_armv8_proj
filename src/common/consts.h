// all opration codes
#ifndef CONST_H
#define CONST_H

/*
The codes are named as operation_[segment_]mnemonic.
Macros beginning with C_ are predicates.
*/

#define BR_COND_EQ      (0b0000)
#define BR_COND_NE      (0b0001)
#define BR_COND_GE      (0b1010)
#define BR_COND_LT      (0b1011)
#define BR_COND_GT      (0b1100)
#define BR_COND_LE      (0b1101)
#define BR_COND_AL      (0b1110)

#define BR_UNCOND       (0b00)
#define BR_COND         (0b01)
#define BR_REG          (0b11)

#define DPI_OPI_AR      (0b010)
#define DPI_OPI_WM      (0b101)

#define DPI_OPC_ADD     (0b00)
#define DPI_OPC_ADDS    (0b01)
#define DPI_OPC_SUB     (0b10)
#define DPI_OPC_SUBS    (0b11)

#define DPI_OPC_MOVN    (0b00)
#define DPI_OPC_MOVZ    (0b10)
#define DPI_OPC_MOVK    (0b11)

#define C_DPR_OPR_MULT(opr)     (0b1000)
#define C_DPR_OPR_AR(opr)       ((opr & 0b1001) == 0b1000)
#define C_DPR_OPR_BL(opr)       ((opr & 0b1000) == 0b0000)

#define DPR_OPC_ADD     (0b00)
#define DPR_OPC_ADDS    (0b01)
#define DPR_OPC_SUB     (0b10)
#define DPR_OPC_SUBS    (0b11)

#define DPR_OPC_AND     (0b00)
#define DPR_OPC_ORR     (0b01)
#define DPR_OPC_EOR     (0b10)
#define DPR_OPC_ANDS    (0b11)

#define SHIFT_LSL       (0b00)
#define SHIFT_LSR       (0b01)
#define SHIFT_ASR       (0b10)
#define SHIFT_ROR       (0b11)

#define LINELENGTH      (400)

#endif
