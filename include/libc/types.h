#ifndef MINI_LIBC_TYPES_H
# define MINI_LIBC_TYPES_H

# ifndef __SIZE_T__
#  define __SIZE_T__
typedef unsigned long int	size_t;
# endif

# ifndef __SSIZE_T__
#  define __SSIZE_T__
typedef long int			ssize_t;
# endif

# ifndef __UINTPTR_T__
#  define __UINTPTR_T__
typedef unsigned long		uintptr_t;
# endif

# ifndef __INTPTR_T__
#  define __INTPTR_T__
typedef long				intptr_t;
# endif

# ifndef __OFF_T__
#  define __OFF_T__
typedef long int	        off_t;
# endif

# ifndef __MODE_T__
#  define __MODE_T__
typedef unsigned int		    mode_t;
# endif

# ifndef __UINT8_T__
# define __UINT8_T__
typedef unsigned char		uint8_t;
# endif

# ifndef __PID_T__
#  define __PID_T__
typedef int					pid_t;
# endif


# ifndef NULL
#  define NULL ((void*)0)
# endif

#endif
