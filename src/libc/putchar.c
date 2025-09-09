#include "../include/libc/unistd.h"

int		x41_putchar(int c)
{
		x41_write(1, &c, 1);
		return 0;
}
