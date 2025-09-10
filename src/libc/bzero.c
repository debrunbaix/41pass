#include "../include/libc/x41_unistd.h"

void x41_bzero(void *s, size_t n) {
    unsigned char *p = s;
    while (n--) {
        *p++ = 0;
    }
}