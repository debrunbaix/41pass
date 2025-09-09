#include "../include/libc/unistd.h"

int		putchar(int c)
{
		write(1, &c, 1);
		return 0;
}
