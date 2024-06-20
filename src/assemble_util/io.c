#include "io.h"

#include "asmconsts.h"
#include "asmline.h"
#include "symbol.h"
#include "../common/datatypes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// pre-assembles from in, outputs to out
int fpreasm(const char *infile, const char *outfile)
{
    FILE *in = fopen(infile, "r");
    FILE *out = fopen(outfile, "wb");
    if (in == NULL)
    {
        if (out != NULL)
        {
            fclose(out);
        }
        return IO_FAILURE;
    }
    if (out == NULL)
    {
        fclose(in);
        return IO_FAILURE;
    }
    char *line = malloc(sizeof(char));
    instr_t buffer[MAX_BUFFER];
    int buffersize;
    int pos = 0;
    size_t linecapp = 0;

    while (1)
    {
        int len;
        if ((len = getline(&line, &linecapp, in)) == -1)
        {
            break;
        }
        line = strsep(&line, "\n\r");
        buffersize = preasmline(line, buffer, pos);
        pos += buffersize;
        fwrite(buffer, sizeof(instr_t), buffersize, out);
    }

    fclose(in);
    fclose(out);

    return pos;
}

// assembles from in, outputs to out
int fasm(const char *infile, const char *outfile)
{
    FILE *in = fopen(infile, "r");
    FILE *out = fopen(outfile, "wb");
    if (in == NULL)
    {
        if (out != NULL)
        {
            fclose(out);
        }
        return IO_FAILURE;
    }
    if (out == NULL)
    {
        fclose(in);
        return IO_FAILURE;
    }

    char *line = malloc(sizeof(char));
    instr_t buffer[MAX_BUFFER];
    int buffersize;
    int pos = 0;
    size_t linecapp = 0;

    while (1)
    {
        int len;
        if ((len = getline(&line, &linecapp, in)) == -1)
        {
            break;
        }
        line = strsep(&line, "\n\r");
        buffersize = asmline(line, buffer, pos);
        pos += buffersize;
        fwrite(buffer, sizeof(instr_t), buffersize, out);
    }

    fclose(in);
    fclose(out);

    return pos;
}
