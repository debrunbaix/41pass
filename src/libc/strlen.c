#include "../include/libc/x41_unistd.h"

size_t x41_strlen(const char *str) {
    size_t len = 0;
    
    if (!str) return 0;
    
    while (*str++) {
        len++;
    }
    return len;
}  