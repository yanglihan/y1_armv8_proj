// all codes
#ifndef CONST_H
#define CONST_H

#define BR_COND_EQ (0b0000)
#define BR_COND_NE (0b0001)
#define BR_COND_GE (0b1010)
#define BR_COND_LT (0b1011)
#define BR_COND_GT (0b1100)
#define BR_COND_LE (0b1101)
#define BR_COND_AL (0b1110)

#define BR_UNCOND (0b00)
#define BR_COND (0b01)
#define BR_REG (0b11)

#define DPI_OPI_AR (0b010)
#define DPI_OPI_WM (0b101)

#define DPI_OPC_ADD (0b00)
#define DPI_OPC_ADDS (0b01)
#define DPI_OPC_SUB (0b10)
#define DPI_OPC_SUBS (0b11)

#define DPI_OPC_MOVN (0b00)
#define DPI_OPC_MOVZ (0b10)
#define DPI_OPC_MOVK (0b11)

#define C_DPR_OPR_MULT(opr) (0b1000)
#define C_DPR_OPR_AR(opr) ((opr & 0b1001) == 0b1000)
#define C_DPR_OPR_BL(opr) ((opr & 0b1000) == 0b0000)

#define DPR_OPC_ADD (0b00)
#define DPR_OPC_ADDS (0b01)
#define DPR_OPC_SUB (0b10)
#define DPR_OPC_SUBS (0b11)

#define DPR_OPC_AND (0b00)
#define DPR_OPC_ORR (0b01)
#define DPR_OPC_EOR (0b10)
#define DPR_OPC_ANDS (0b11)

#endif
