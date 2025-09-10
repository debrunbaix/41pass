#ifndef	MINI_LIBC_UNISTD_H
#define	MINI_LIBC_UNISTD_H

#include "types.h"
#include "x41_stdarg.h"
#include "signal.h"
#include "sys/x41_wait.h"

/* Standard descriptor. */
#define	STDIN	0
#define	STDOUT	1
#define STDERR	2
#define	ERROR	-1
#define	SUCCESS	0

ssize_t	x41_write(int fd, const void *buf, size_t count);
int		x41_putchar(int c);
int		x41_puts(const char *str);
int		x41_putnbr(int num);
int		x41_strcmp(const char *s1, const char *s2);
int		x41_strncmp(const char *s1, const char *s2, size_t n);
ssize_t	x41_read(int fd, void *buf, size_t count);
void	x41_exit(int status);
int		x41_printf(const char *format, ...);
void	*x41_sys_brk(void *addr);
int		x41_brk(void *addr);
void	*x41_sbrk(int increment);
char	*x41_strcpy(char *dest, const char *src);
void	*x41_malloc(size_t size);
void	x41_free(void *ptr);
void	*x41_realloc(void *ptr, size_t size);
ssize_t	x41_getline(char **lineptr, size_t *n, int fd);
int		x41_starts_with(const char *s, const char *prefix);
int		x41_execve(const char *pathname, char *const argv[], char *const envp[]);
pid_t	x41_fork(void);
pid_t	x41_getpid(void);
pid_t	x41_waitpid(pid_t pid, int *wstatus, int options);
int		x41_pipe(int pipefd[2]);
int		x41_dup2(int oldfd, int newfd);
int		x41_close(int fd);
int		x41_open(const char *filename, int flags, mode_t mode);

#endif
