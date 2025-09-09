#include "../include/libc/unistd.h"

ssize_t	x41_read(int fd, char *buf, size_t count)
{
		long ret;

		__asm__ volatile(
				"syscall"
				: "=a"(ret)
				: "a"(0),
				  "D"(fd),
				  "S"(buf),
				  "d"(count)
				: "rcx", "r11", "memory"
		);
		if (ret < 0) return -1;
		return ret;
}
