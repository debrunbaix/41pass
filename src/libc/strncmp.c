#include "../include/libc/x41_unistd.h"

int x41_strncmp(const char *s1, const char *s2, size_t n)
{
    while (n-- > 0 && *s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

