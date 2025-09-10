#include "../include/libc/x41_unistd.h"
#include "../include/libc/x41_errno.h" 

/*
 *   https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html
 *   https://gcc.gnu.org/onlinedocs/gcc/Constraints.html
 *   https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#Clobbers-and-Scratch-Registers
 *
 *   Types defined in the header:
 *   typedef unsigned long long size_t;
 *   typedef long long          ssize_t;
 *   typedef unsigned int      mode_t;
*/

int x41_open(const char *pathname, int flags, mode_t mode) {
    long ret;
    __asm__ volatile (
        "syscall\n"
        : "=a" (ret)           /*rax return value */
        : "a"(2),              /* syscall 2: open */
          "D"((long)pathname), /* rdi 1st arg: pathname */
          "S"(flags),          /* rsi 2nd arg: flags */
          "d"((long)mode)           /* rdx 3rd arg: mode */
        : "rcx", "r11", "memory"  /* clobbered registers */
    );

    if (ret < 0) {
        x41_errno = (int)(-ret);
        return (-1);
    }
    x41_errno = 0;
    return ret;
}

