// utilities used in assembler
#ifndef ASMUTIL_H
#define ASMUTIL_H

#include "../common/datatypes.h"

#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

// truncation operations
#define utrun64(opr, to)  (uint64_t)((uint64_t)(opr) << (64 - (to))) >> (64 - (to))
#define trun64(opr, to)  (uint64_t)((int64_t)(opr) << (64 - (to))) >> (64 - (to))

// trims a string from the left
extern void ltrim(char **chrptr);

// trims a string from the right
extern void rtrim(char *chrptr, size_t len);

// copy a string from src to dst while unescaping src
extern int cpyunesc(char *src, char *dst);

// inserts n elements at pos in an array, assuming sufficient size, returns the new size
extern size_t insnelem(seg_t *dst, size_t len, size_t pos, seg_t *src, size_t n);

#endif
