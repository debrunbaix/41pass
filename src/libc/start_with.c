#include "../include/libc/unistd.h"

int		starts_with(const char *s, const char *prefix)
{
		size_t	i = 0;

		while (prefix[i])
		{
				if (s[i] != prefix[i]) return 0;
				i++;
		}
		return 1;
}
