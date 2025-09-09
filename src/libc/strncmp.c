#include "../include/libc/unistd.h"

int strncmp(const char *s1, const char *s2, size_t n)
{
		int i = 0;

		for (i = 0; (s1[i] != '\0' || s2[i] != '\0') && (i <= n); i++)
		{
				if (s1[i] < s2[i]) return -i;
				if (s1[i] > s2[i]) return i;
		}
		return 0;
}

