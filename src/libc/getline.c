#include "../include/libc/x41_unistd.h"     // read, ssize_t, size_t
#include "../include/libc/allocator.h"  // malloc, realloc

#define GL_INIT_CAP 128

ssize_t x41_getline(char **lineptr, size_t *n, int fd)
{
		if (!lineptr || !n) return -1;

		if (*lineptr == 0 || *n == 0) {
				*n = GL_INIT_CAP;
				*lineptr = (char *)x41_malloc(*n);
				if (!*lineptr)
				{ 
						*n = 0; 
						return -1; 
				}
		}

		size_t pos = 0;

		for (;;) {
				char ch;
				ssize_t r = x41_read(fd, &ch, 1);
				if (r < 0)
				{
					if (pos == 0) return -1;
					break;
				}
				if (r == 0)
				{
					if (pos == 0) return -1;
					break;
				}

				if (pos + 1 >= *n) {
						size_t new_cap = (*n < GL_INIT_CAP) ? GL_INIT_CAP : (*n * 2);
						char *p = (char *)x41_realloc(*lineptr, new_cap);
						if (!p)
						{
								(*lineptr)[pos] = '\0';
								return (ssize_t)pos;
						}
						*lineptr = p;
						*n = new_cap;
				}

				(*lineptr)[pos++] = ch;
				if (ch == '\n') break;
		}

		(*lineptr)[pos] = '\0';
		return (ssize_t)pos;
}
