#include "../include/libc/x41_unistd.h"

void	*x41_sbrk(int increment)
{
		void	*current = x41_sys_brk(0);

		if (increment == 0) return current;

		uintptr_t	current_u = (uintptr_t)current;
		uintptr_t	increment_u = (uintptr_t)increment;
		uintptr_t	new_addr_u = current_u + increment_u;
		void	*new_addr = (void *)new_addr_u;
		
		void	*ret = x41_sys_brk(new_addr);

		if ((uintptr_t)ret < new_addr_u) return (void *)-1;
		return current;
}
