#include "../include/libc/unistd.h"

pid_t waitpid(pid_t pid, int *wstatus, int options)
{
		long	ret;
		register void *r10 __asm__("r10") = 0;

		__asm__ volatile(
				"syscall"
				: "=a"(ret)
				: "a"(61),
				  "D"(pid),
				  "S"(wstatus),
				  "d"(options),
				  "r"(r10)
				: "rcx", "r11", "memory"
		);
		return (pid_t)ret;
}

