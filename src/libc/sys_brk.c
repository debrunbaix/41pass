#include "../include/libc/x41_unistd.h"

void	*x41_sys_brk(void *addr)
{
		void	*ret;

		__asm__ volatile(
				"syscall"
				: "=a"(ret)
				: "a"(12),
				  "D"(addr)
				: "rcx", "r11", "memory"
		);
		return ret;
}
