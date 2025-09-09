#include "../include/libc/unistd.h"
#define INTTOCHAR 48

int		get_reversed_number(int num)
{
		int reversed_nb = 0;

		while(num > 0)
		{
				int mod = num % 10;
				reversed_nb = reversed_nb * 10 + mod;
				num = num / 10;
		}
		return reversed_nb;
}

int		putnbr(int num)
{
		num = get_reversed_number(num);

		while(num > 0)
		{
				int mod = num % 10;
				mod = mod + INTTOCHAR;
				write(1, &mod, 1);
				num = num / 10; 
		}
}
