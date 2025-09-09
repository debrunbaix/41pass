#include "../include/libc/unistd.h"

int		brk(void *addr)
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

