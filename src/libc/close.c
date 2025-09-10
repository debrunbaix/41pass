#include "../include/libc/x41_unistd.h"
#include "../include/libc/x41_errno.h" 


int x41_close(int fd)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(3), "D"(fd)
        : "rcx", "r11", "memory"
    );
        if (ret < 0) {
        x41_errno = (int)(-ret);
        return (-1);
    }
    x41_errno = 0;
    return ret;
}

