#include "../include/libc/unistd.h"

pid_t	x41_fork(void)
{
		long	ret;

		__asm__ volatile(
				"syscall"
				: "=a"(ret)
				: "a"(57)
				: "rcx", "r11", "memory"
		);
		return (pid_t)ret;
}
