#include "../include/libc/x41_unistd.h"

int		x41_puts(const char *str)
{
		int i;

		for (i = 0; str[i] != '\0'; i++)
		{
				x41_putchar(str[i]);
		}
		x41_putchar('\n');
		return 0;
}
