#include "../include/libc/x41_unistd.h"

int		x41_brk(void *addr)
{
		void	*ret;

		__asm__ volatile(
				"syscall"
				: "=a"(ret)
				: "a"(12),
				  "D"(addr)
				: "rcx", "r11", "memory"
		);
		if ((uintptr_t)ret < (uintptr_t)addr) return ERROR;
		return SUCCESS;
}

