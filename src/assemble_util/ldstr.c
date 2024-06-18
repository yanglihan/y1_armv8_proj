#include "ldstr.h"

#include "asmutil.h"

// calculates offset
static int calculate_offset(const char *label)
{

    return 0;
}

// converts a load/store operation to binary
instr_t ldstr(char **args, int argc, int8_t load)
{
    assert(argc == 2 || argc == 3);

    char *ptr;
    seg_t sf = args[0][0] == 'x';
    seg_t rt = strtol(&args[0][1], &ptr, 10);
    seg_t xn = 0;
    seg_t offset = 0;
    seg_t l = load ? 1 : 0;

    if (args[1][0] == '[')
    {
        char *addr = args[1];
        if (sscanf(addr, "[x%d]", &xn) == 1)
        {
            offset = 0;
            return sdt(sf, l, offset, xn, rt);
        }
        else if (sscanf(addr, "[x%d, #%d]!", &xn, &offset) == 2)
        {
            return sdt(sf, l, offset, xn, rt);
        }
        else if (sscanf(addr, "[x%d], #%d", &xn, &offset) == 2)
        {
            return sdt(sf, l, offset, xn, rt);
        }
        else if (sscanf(addr, "[x%d, x%d]", &xn, &offset) == 2)
        {
            return sdt(sf, l, offset, xn, rt);
        }
        else
        {
            assert(0);
            // printf("Invalid Address Format\n");
            return 0;
        }
    }
    else // literal address mode
    {
        char *offsetstr = args[1];
        offset = calculate_offset(offsetstr);
        return sdt(sf, l, offset, xn, rt);
    }
}
