#include "../include/libc/unistd.h"

int	puts_printf(const char *str)
{
		int i;

		for (i = 0; str[i] != '\0'; i++)
		{
				putchar(str[i]);
		}
		return 0;
}

int		printf(const char *format, ...)
{
		va_list	ap;
		va_start(ap, format);

		for (const char *current_char = format; *current_char; ++current_char)
		{
				if (*current_char != '%')
				{
						putchar(*current_char);
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
								putchar((char)ch);
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
								putnbr(num);
								break;
						}
						default:
						{
								putchar('%');
								putchar(*current_char);
								break;
						}
				}
		}
		va_end(ap);
		return 0;
}
