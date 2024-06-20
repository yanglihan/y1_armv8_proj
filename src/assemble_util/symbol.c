#include "symbol.h"

#include "asmconsts.h"
#include "asmutil.h"
#include <stdlib.h>

#define FORALLSYMB_I for (int i = 0; i < nsymb; i++)

// stores one label and its address
typedef struct symb {
    char label[21];
    addr_t addr;
} symb_t;

symb_t *symbtbl; // the symbol table
int nsymb = 0;  // the number of symbols

// add a label-address pair into the table, auto-remove the colon
int symbadd(char *label, addr_t addr)
{
    char *colon;
    assert(!existlabel(label));
    symbtbl = realloc(symbtbl, sizeof(symb_t) * (nsymb + 1));
    symbtbl[nsymb].addr = addr;
    strncpy(symbtbl[nsymb].label, label, 20);
    colon = strchr(symbtbl[nsymb].label, ':');
    if (colon)
    {
        *colon = '\0';
    }
    nsymb++;
    return 0;
}

// check if a label already exists
int8_t existlabel(char *label)
{
    FORALLSYMB_I
    {
        if (!strcmp(symbtbl[i].label, label))
        {
            return 1;
        }
    }
    return 0;
}

// get the address of a label
addr_t symbget(char *label, int len)
{
    FORALLSYMB_I
    {
        if (!strncmp(symbtbl[i].label, label, len))
        {
            return symbtbl[i].addr;
        }
    }
    assert(0);
    return 0;
}

// free the symbol table
void freesymbtbl()
{
    free(symbtbl);
}

