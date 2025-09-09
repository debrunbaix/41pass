#include"errno.h"
#if defined(__GNUC__)
    __thread int x41_errno;
#else
    #error "Thread‑local storage errno not implemented"
#endif
