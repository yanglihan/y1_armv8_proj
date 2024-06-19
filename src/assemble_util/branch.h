// branch operation assembler
#ifndef BRANCH_H
#define BRANCH_H

#include "../common/datatypes.h"

// converts a branch operation to binary
extern instr_t branch(int *argv, int argc, seg_t cond);

#endif
