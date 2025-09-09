#include "../include/libc/x41_unistd.h"

pid_t	x41_getpid(void)
{
		long	ret;

		__asm__ volatile(
				"syscall"
				: "=a"(ret)
				: "a"(39)
				: "rcx", "r11", "memory"
		);
		return (pid_t)ret;
}
