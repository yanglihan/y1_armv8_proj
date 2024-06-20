#include "asmutil.h"

// trims a string from the left
void ltrim(char **chrptr)
{
    for (; **chrptr && isspace(**chrptr); (*chrptr)++);
}

// trims a string from the right
void rtrim(char *chrptr, size_t len)
{
    for (char *ptr = chrptr + len - 1; ptr >= chrptr && isspace(*ptr); *ptr-- = '\0');
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
    memmove(dst + pos + n, dst + pos, (len - pos) * sizeof(seg_t));
    memcpy(dst + pos, src, n * sizeof(seg_t));
    return len + n;
}
