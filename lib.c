#include "lib.h"

void memcpy(char *dest, char *src, u32 size)
{
    int i;
    for (i = 0; i < size; ++i)
        dest[i] = src[i];
}
