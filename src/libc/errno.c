#include"../include/libc/x41_errno.h"
#if defined(__GNUC__)
    #ifdef TLS_ENABLED
        extern __thread int x41_errno;
    #else 
        int x41_errno;
    #endif
#else
    #error "Thread‑local storage errno not implemented"
#endif
