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
    printf("fpreasm entered %s => %s\n", infile, outfile); // debug
    FILE *in = fopen(infile, "r");
    FILE *out = fopen(outfile, "wb");
    if (in == NULL)
    {
        printf("fpreasm: input failed\n"); // debug
        if (out != NULL)
        {
            fclose(out);
        }
        printf("fpreasm: output failed\n"); // debug
        return IO_FAILURE;
    }
    if (out == NULL)
    {
        printf("fpreasm: output failed\n"); // debug
        fclose(in);
        return IO_FAILURE;
    }
    printf("fpreasm: files opened successfully\n"); // debug

    char *line = malloc(sizeof(char));
    instr_t buffer[MAX_BUFFER];
    int buffersize;
    int pos = 0;
    size_t linecapp = 0;

    printf("fpreasm: starting loop...\n"); // debug

    while (1)
    {
        int len;
        if ((len = getline(&line, &linecapp, in)) == -1)
        {
            printf("fpreasm: EOF\n"); // debug
            break;
        }
        printf("fpreasm: read line %s", line); // debug
        line = strsep(&line, "\n\r");
        printf("fpreasm: **read line %s\n", line); // debug
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
    printf("fasm entered %s => %s\n", infile, outfile); // debug
    FILE *in = fopen(infile, "r");
    FILE *out = fopen(outfile, "wb");
    if (in == NULL)
    {
        printf("fasm: input failed\n"); // debug
        if (out != NULL)
        {
            fclose(out);
        }
        printf("fasm: output failed\n"); // debug
        return IO_FAILURE;
    }
    if (out == NULL)
    {
        printf("fasm: output failed\n"); // debug
        fclose(in);
        return IO_FAILURE;
    }
    printf("fasm: files opened successfully\n"); // debug

    char *line = malloc(sizeof(char));
    instr_t buffer[MAX_BUFFER];
    int buffersize;
    int pos = 0;
    size_t linecapp = 0;

    printf("fasm: starting loop...\n"); // debug

    while (1)
    {
        int len;
        if ((len = getline(&line, &linecapp, in)) == -1)
        {
            printf("fasm: EOF\n"); // debug
            break;
        }
        printf("fasm: read line %s", line); // debug
        line = strsep(&line, "\n\r");
        printf("fasm: **read line %s\n", line); // debug
        buffersize = asmline(line, buffer);
        pos += buffersize;
        printf("fasm: to write %u x %u\n", buffersize, sizeof(instr_t)); // debug
        fwrite(buffer, sizeof(instr_t), buffersize, out);
    }

    fclose(in);
    fclose(out);

    return pos;
}
