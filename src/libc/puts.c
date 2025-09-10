#include "../include/libc/x41_unistd.h"
#include "../include/libc/x41_errno.h"

int		x41_puts(const char *str)
{
    int i = 0;
    if (!str){
        x41_errno = EFAULT;
        return (-1);
    }
    while (str[i])
        i++;
    if (x41_write(1, str, i) < 0) 
        return (-1); // return E0F on error
    if (x41_putchar('\n') == -1)
        return (-1);
    return (++i);
}
