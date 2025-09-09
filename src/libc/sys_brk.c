#include "../include/libc/unistd.h"

void	*sys_brk(void *addr)
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
