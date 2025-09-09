#include "../include/libc/x41_unistd.h"

int		x41_execve(const char *pathname, char *const argv[], char *const envp[])
{
		long	ret;

		__asm volatile(
				"syscall"
				: "=a"(ret)
				: "a"(59), "D"(pathname), "S"(argv), "d"(envp)
				: "rcx", "r11", "memory"
		);
		return (int)ret;
}
