#include "state.h"

reg_t r[32], * zr = r + 31, pc; // registers

pstate_t pstate = { 0, 1, 0, 0 }; // p-states
