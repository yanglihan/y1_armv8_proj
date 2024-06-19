// arithmetic operation assembler
#ifndef ARITH_H
#define ARITH_H

#include "../common/datatypes.h"

// converts an arithmetic operation to binary
extern instr_t arith(seg_t *argv, int argc, seg_t opc);

#endif
