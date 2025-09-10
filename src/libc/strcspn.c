#include "../include/libc/x41_unistd.h"

size_t x41_strcspn(const char *s, const char *reject){
    size_t i = 0;
    const char *r = reject;
    
    while (s[i]) {
        r = reject;
        while (*r) {
            if (s[i] == *r) {
                return i;
            }
            r++;
        }
        i++;
    }
    return i;
}
