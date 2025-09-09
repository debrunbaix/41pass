#include "../include/libc/x41_unistd.h"

int x41_dup2(int oldfd, int newfd)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(33), "D"(oldfd), "S"(newfd)
        : "rcx", "r11", "memory"
    );
    return (int)ret; /* new fd >=0 or -1 on error */
}

