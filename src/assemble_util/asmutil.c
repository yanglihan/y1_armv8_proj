#include "asmutil.h"

// trims a string from the left
void ltrim(char **chrptr)
{
    for (; **chrptr && isspace(**chrptr); (*chrptr)++)
        ;
}

// copy a string from src to dst while unescaping src
int cpyunesc(char *src, char *dst)
{
    char *dstptr = dst;
    for (; *src; src++)
    {
        if (*src == '\\')
        {
            switch (*++src)
            {
            case '\\':
                *dstptr++ = '\\';
                break;
            case 't':
                *dstptr++ = '\t';
                break;
            case '\"':
                *dstptr++ = '\"';
                break;
            case 'b':
                *dstptr++ = '\b';
                break;
            case 'n':
                *dstptr++ = '\n';
                break;
            case 'r':
                *dstptr++ = '\r';
                break;
            case '0':
                *dstptr++ = '\0';
            default: // escape to hex or octal not implemented
                *dstptr++ = '\\';
                *dstptr++ = *src;
                break;
            }
            continue;
        }
        if (*src == '"')
        {
            *dstptr++ = '\0';
            break;
        }

        *dstptr++ = *src;
    }
    return dstptr - dst;
}

// inserts n elements at pos in an array, assuming sufficient size, returns the new size
size_t insnelem(seg_t *dst, size_t len, size_t pos, seg_t *src, size_t n)
{
    printf("insnelem: attempting to insert elements %llu, %llu\n", src[0], src[1]); // debug
    memmove(dst + pos + n, dst + pos, (len - pos) * sizeof(seg_t));
    memcpy(dst + pos, src, n * sizeof(seg_t));
    printf("insnelem: resulting elements %llu, %llu\n", dst[pos], src[pos + 1]); // debug
    return len + n;
}
