#include "../include/libc/x41_unistd.h"
#include "../include/libc/colors.h"

char *x41_readline(const char *prompt)
{
        char	*line = NULL;
        size_t	len = 0;

        if (prompt) x41_printf(COLOR_CYAN "%s" COLOR_RESET, prompt);
        if (x41_getline(&line, &len, 0) > 0)
                return line;
        if (line) x41_free(line);
        return NULL;
}