// logical operation compiler
#ifndef LOGICS_H
#define LOGICS_H

#include "../common/datatypes.h"

// converts a logical operation to binary
extern instr_t logic(char **args, int argc, seg_t opc, seg_t n);

#endif
