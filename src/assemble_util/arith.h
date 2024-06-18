// arithmetic operation assembler
#ifndef ARITH_H
#define ARITH_H

#include "../common/datatypes.h"

// converts an arithmetic operation to binary
extern instr_t arith(char **args, int argc, int8_t flg, int8_t sub);

#endif
