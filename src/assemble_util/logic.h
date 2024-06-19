// logical operation assembler
#ifndef LOGICS_H
#define LOGICS_H

#include "../common/datatypes.h"

// converts a bit-wise logical operation to binary
extern instr_t logic(int *argv, int argc, seg_t opc, seg_t n);

#endif
