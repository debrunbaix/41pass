#include "../include/libc/x41_unistd.h"

int	puts_printf(const char *str)
{
		int i;

		for (i = 0; str[i] != '\0'; i++)
		{
				x41_putchar(str[i]);
		}
		return 0;
}

int		x41_printf(const char *format, ...)
{
		va_list	ap;
		va_start(ap, format);

		for (const char *current_char = format; *current_char; ++current_char)
		{
				if (*current_char != '%')
				{
						x41_putchar(*current_char);
						continue;
				}
				++current_char;
				if (*current_char == '\0') 
						break;
				switch (*current_char)
				{
						case 'c':
						{
								int	ch = va_arg(ap, int);
								x41_putchar((char)ch);
								break;
						}
						case 's':
						{
								const char	*s = va_arg(ap, const char *);
								if (!s) s = "(null)";
								puts_printf(s);
								break;
						}
						case 'd':
						{
								int	num = va_arg(ap, int);
								x41_putnbr(num);
								break;
						}
						default:
						{
								x41_putchar('%');
								x41_putchar(*current_char);
								break;
						}
				}
		}
		va_end(ap);
		return 0;
}
