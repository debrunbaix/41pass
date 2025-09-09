#include "../include/libc/unistd.h"

int		open(const char *filename, int flags, umode_t mode)
{
		long	ret;
		__asm__ volatile(
				"syscall"
				: "=a"(ret)
				: "a"(2), "D"(filename), "S"(flags), "d"(mode)
				: "rcx", "r11", "memory"
		);
		return (int)ret;
}

