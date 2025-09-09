#include "../include/libc/x41_unistd.h"
#include "../include/libc/x41_errno.h"

/*
 *   https://gcc.gnu.org/onlinedocs/gcc/Constraints.html
 *   https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#Clobbers-and-Scratch-Registers
 *   Types defined in the header:
 *   typedef unsigned long long size_t;
 *   typedef long long          ssize_t;
*/

ssize_t x41_read(int  fd, void *buf, size_t count) {
    long ret;
    __asm__ volatile (
        "syscall\n"
        : "=a" (ret)     /*rax return value */
        : "a"(0),        /* syscall 0: read */
          "D"((long)fd), /* rdi 1st arg: file descriptor */
          "S"(buf),      /* rsi 2nd arg: buffer */
          "d"(count)     /* rdx 3rd arg: count */
        : "rcx", "r11", "memory"  /* clobbered registers */
    );

    if (ret < 0) {
        x41_errno = (int)(-ret);
        return (-1);
    }
    x41_errno = 0;
    return ret;
}

