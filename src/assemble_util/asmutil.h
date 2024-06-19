// utilities
#ifndef ASMUTIL_H
#define ASMUTIL_H

#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "../common/datatypes.h"

// #define isalpha(c) (((c) >= 'a' || (c) <= 'z') || ((c) >= 'A' || (c) <= 'Z'))

// trims a string from the left
extern void ltrim(char **chrptr);

// copy a string from src to dst while unescaping src
extern int cpyunesc(char *src, char *dst);

// inserts n elements at pos in an array, assuming sufficient size, returns the new size
extern size_t insnelem(seg_t *dst, size_t len, size_t pos, seg_t *src, size_t n);

#endif
