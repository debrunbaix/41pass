#include "../include/libc/x41_unistd.h"

int		x41_putchar(int c)
{
		return ((int)x41_write(1, &c, 1));
}
