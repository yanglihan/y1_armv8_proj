// branch operation assembler
#ifndef BRANCH_H
#define BRANCH_H

#include "../common/datatypes.h"

// converts a branch operation to binary
extern instr_t branch(seg_t *argv, int argc, seg_t cond);

#endif
