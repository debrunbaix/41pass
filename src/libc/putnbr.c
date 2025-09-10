#include "../include/libc/x41_unistd.h"

int x41_putnbr(int num){
    char buf[12] = {0}; // 11 for the sign and null terminator
    const int sign = (num < 0) ? -1 : 1;
    int i = sizeof(buf);

    while (i && (num > 9 || num < -9)) {
        buf[--i] = (num % 10) * sign + '0';
        num /= 10;
    }
    buf[--i] = num * sign + '0';
    if (sign < 0)
        buf[--i] = '-';
    return(x41_write(1, buf + i, sizeof(buf) - i));
}