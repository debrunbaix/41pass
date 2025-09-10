#include "../include/libc/x41_unistd.h"
#include "../include/libc/x41_errno.h"
#include "../include/libc/x41_stdarg.h"

int x41_printf(const char *format, ...) {
    int ret = 0;
    int len = 0;
    va_list args;
    va_start(args, format);

    if (!format) {
        x41_errno = EINVAL;
        return (-1);
    }
    
    for (const char *p = format; *p != '\0'; ++p) {
        if (*p != '%') {
            if (x41_putchar (*p) < 0) // If output error 
                ret = -1;
            else if (ret != -1)
                ret++; 
            continue;
        }

        ++p;
        if (*p == '\0')
            break ;

        switch (*p) {
            case 's': { 
                const char *s = va_arg(args, const char *);
                if (!s)
                    s = "(null)";
                while (*s) { //Cannot use puts because puts prints %s\n 
                    if (x41_putchar(*s++) < 0) // If output error 
                        ret = -1;
                    else if (ret != -1)
                        ret++; 
                }
                break ;
            }
            case 'd': {
                int d =  va_arg(args, int);
                if ((len = x41_putnbr(d)) < 0) // If output error 
                    ret = -1;
                else if (ret != -1){
                    len = 0;
                    if (d < 0)
                        len++; // account for the '-' sign
                    while (d > 9 || d < -9) {
                        len++;
                        d /= 10;
                    }
                    len++; // account for the last digit
                    ret += len;
                }
                    
                break;
            }
            case 'c': {
                unsigned char c = (unsigned char)va_arg(args, int);
                if (x41_putchar(c) < 0) // If output error 
                    ret = -1;
                else if (ret != -1)
                    ret++; 
                break;
            }
            case '%': {
                if (x41_putchar('%') < 0) // If output error 
                    ret = -1;
                else if (ret != -1)
                    ret++; 
                break;
            }
            default: { /* unknown format speficier */
                if (x41_putchar('%') < 0) // If output error 
                    ret = -1;
                else if (ret != -1)
                    ret++; 
                if (x41_putchar(*p) < 0) // If output error 
                    ret = -1;
                else if (ret != -1)
                    ret++; 
                break;
            }
        }

    }

    va_end(args);
    return (ret);
}