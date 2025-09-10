#include "../include/libc/x41_unistd.h"
#include "../include/libc/x41_errno.h"

char *x41_strdup(const char *s) {
    if (!s) {
        x41_errno = EFAULT;
        return NULL;
    }
    
    size_t len = 0;
    while (s[len]) len++;
    
    char *dup = (char *)x41_malloc(len + 1);
    if (!dup) {
        x41_errno = ENOMEM;
        return NULL;
    }
    
    for (size_t i = 0; i <= len; i++) {
        dup[i] = s[i];
    }
    
    return dup;
}