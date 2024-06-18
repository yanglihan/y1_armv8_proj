#include "symbol.h"

#include "asmconst.h"
#include "asmutil.h"
#include "../common/datatypes.h"
#include <stdio.h>
#include <stdlib.h>

#define FORALLSYMB_I for (int i = 0; i < nsymb; i++)

// stores one label and its address
typedef struct symb {
    char label[20];
    addr_t addr;
} symb_t;

symb_t *symbtbl; // the symbol table
int nsymb = 0;  // the number of symbols

int symbload(FILE *src)
{
    char line[40];
    int const maxlength = LINE_MAX_LENGTH;
    for (int size; size = getline(line, &maxlength, src); size != -1)
    {
        if (size == 0)
        {
            continue;
        }
        
        if (line[size - 2] == ':' && isalpha(line[0]))
        {
            char label[20];
            
        }
        
    }
    return 0;
}

int8_t existlabel(char *label)
{
    FORALLSYMB_I
    {
        if (strcmp(symbtbl[i].label, label))
        {
            return 1;
        }
    }
    return 0;
}

addr_t symbget(char *label)
{
    FORALLSYMB_I
    {
        if (strcmp(symbtbl[i].label, label))
        {
            return symbtbl[i].addr;
        }
    }
    assert(0);
    return 0;
}

void freesymbtbl()
{
    free(symbtbl);
}
