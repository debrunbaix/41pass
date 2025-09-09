#ifndef MINI_LIBC_TYPES_H
#define MINI_LIBC_TYPES_H

typedef unsigned long long	size_t;
typedef long long			ssize_t;

typedef unsigned long		uintptr_t;
typedef long				intptr_t;

typedef unsigned short		umode_t;
typedef unsigned char		uint8_t;

typedef int					pid_t;

#ifndef NULL
# define NULL ((void*)0)
#endif

#endif
