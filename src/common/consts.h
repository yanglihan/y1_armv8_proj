// all opration codes
#ifndef CONST_H
#define CONST_H

/*
The codes are named as operation_[segment_]mnemonic.
Macros beginning with C_ are predicates.
*/

#define DEFAULT_NO_ARG  (0)

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

#define BR_TMPL_UNC     (0x14000000)
#define BR_TMPL_REG     (0xd61f0000)
#define BR_TMPL_CON     (0x54000000)

#define DPI_OPI_AR      (0b010)
#define DPI_OPI_WM      (0b101)

#define DPI_OPC_MOVN    (0b00)
#define DPI_OPC_MOVZ    (0b10)
#define DPI_OPC_MOVK    (0b11)

#define DPI_SH_NOSHIFT  (0b0)
#define DPI_SH_SHIFT    (0b1)

#define C_DPR_OPR_MUL(opr)     (opr == 0b1000)
#define C_DPR_OPR_AR(opr)       ((opr & 0b1001) == 0b1000)
#define C_DPR_OPR_BL(opr)       ((opr & 0b1000) == 0b0000)

#define DPR_OPR_MUL    (0b1000)
#define DPR_OPR_ARITH   (0b1000)
#define DPR_OPR_LOGIC   (0b0000)

#define DPR_OPC_AND     (0b00)
#define DPR_OPC_ORR     (0b01)
#define DPR_OPC_EOR     (0b10)
#define DPR_OPC_ANDS    (0b11)

#define DPR_M_NOMUL     (0b0)
#define DPR_M_MUL       (0b1)

#define SDT_OFFS_TMPL_REG    (0x81a)
#define SDT_OFFS_TMPL_PRE    (0x003)
#define SDT_OFFS_TMPL_POS    (0x001)
#define SDT_OFFS_TMPL_IMM    (0x000)

#define SDT_U_SIGNED    (0b0)
#define SDT_U_UNSIGNED  (0b1)

#define SHIFT_LSL       (0b00)
#define SHIFT_LSR       (0b01)
#define SHIFT_ASR       (0b10)
#define SHIFT_ROR       (0b11)

#define LDSTR_STR       (0b0)
#define LDSTR_LD        (0b1)

#define LOGIC_NONEG     (0b0)
#define LOGIC_NEG       (0b1)

#define ARITH_OPC_ADD   (0b00)
#define ARITH_OPC_ADDS  (0b01)
#define ARITH_OPC_SUB   (0b10)
#define ARITH_OPC_SUBS  (0b11)

#define ARITH_NONEG     (0b0)
#define ARITH_NEG       (0b1)

#endif
