// branch operation compiler
#ifndef BRANCH_H
#define BRANCH_H

#include "../common/datatypes.h"

// converts a branch operation to binary
extern instr_t branch(char **args, int argc);

#endif
