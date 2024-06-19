#ifndef LDSTR_H
#define LDSTR_H

#include "../common/datatypes.h"

// converts a load/store operation to binary
extern instr_t ldstr(int *argv, int argc, seg_t load);

#endif
