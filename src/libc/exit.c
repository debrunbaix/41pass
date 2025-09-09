#include "../include/libc/unistd.h"

void	exit(int status)
{
		__asm__ volatile(
				"syscall"
				: 
				: "a"(60),
				  "D"(status & 0xff)
				: "rcx", "r11", "memory"
		);
}
