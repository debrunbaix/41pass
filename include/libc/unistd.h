#ifndef	MINI_LIBC_UNISTD_H
#define	MINI_LIBC_UNISTD_H

#include "types.h"
#include "stdarg.h"
#include "signal.h"
#include "sys/wait.h"

/* Standard descriptor. */
#define	STDIN	0
#define	STDOUT	1
#define STDERR	2
#define	ERROR	-1
#define	SUCCESS	0

ssize_t	write(int fd, const void *buf, size_t count);
int		putchar(int c);
int		puts(const char *str);
int		putnbr(int num);
int		strcmp(const char *s1, const char *s2);
int		strncmp(const char *s1, const char *s2, size_t n);
ssize_t	read(int fd, char *buf, size_t count);
void	exit(int status);
int		printf(const char *format, ...);
void	*sys_brk(void *addr);
int		brk(void *addr);
void	*sbrk(int increment);
char	*strcpy(char *dest, char *src);
void	*malloc(size_t size);
void	free(void *ptr);
void	*realloc(void *ptr, size_t size);
ssize_t	getline(char **lineptr, size_t *n, int fd);
int		starts_with(const char *s, const char *prefix);
int		execve(const char *pathname, char *const argv[], char *const envp[]);
pid_t	fork(void);
pid_t	getpid(void);
pid_t	waitpid(pid_t pid, int *wstatus, int options);
int pipe(int pipefd[2]);
int dup2(int oldfd, int newfd);
int close(int fd);
int		open(const char *filename, int flags, umode_t mode);

#endif
