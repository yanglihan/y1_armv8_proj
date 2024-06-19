// wide move operation assembler
#ifndef MOVE_H
#define MOVE_H

#include "../common/datatypes.h"

// converts a wide move operation to binary
extern instr_t move(int *argv, int argc, seg_t opc);

#endif
