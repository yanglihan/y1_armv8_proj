#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdio.h>
#include <stdlib.h>

#include "emulator_util/datatypes.h"
#include "emulator_util/memory.h"
#include "emulator_util/interpret.h"
#include "emulator_util/io.h"

// #define LINE_BY_LINE_PRINT_DEBUG // to print the resulting state after each instruction (warning: affects performance)
// #define DEBUG                    // for general debugging

#ifdef LINE_BY_LINE_PRINT_DEBUG
#define debug_print_state() print_state()
#else
#define debug_print_state()
#endif

#ifdef DEBUG
#define debug_printf(fstr, ...) printf(fstr, __VA_ARGS__)
#else
#define debug_printf(fstr, ...)
#endif

#endif
