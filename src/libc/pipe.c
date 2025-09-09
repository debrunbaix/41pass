#include "../include/libc/x41_unistd.h"

int x41_pipe(int pipefd[2])
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(22), "D"(pipefd)
        : "rcx", "r11", "memory"
    );
    return (int)ret; /* 0 on success, -1 on error */
}

