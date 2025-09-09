#include "../include/libc/unistd.h"

int		puts(const char *str)
{
		int i;

		for (i = 0; str[i] != '\0'; i++)
		{
				putchar(str[i]);
		}
		putchar('\n');
		return 0;
}
