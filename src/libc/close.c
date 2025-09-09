#include "../include/libc/unistd.h"

int x41_close(int fd)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(3), "D"(fd)
        : "rcx", "r11", "memory"
    );
    return (int)ret; /* 0 or -1 */
}

