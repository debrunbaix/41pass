#include "../include/libc/unistd.h"

pid_t	getpid(void)
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
