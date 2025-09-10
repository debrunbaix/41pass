#include	"../include/libc/x41_unistd.h"
#include	"pwman.h"

int		main(int argc, char **argv, char **envp)
{
		if (argc < 3) return ERROR;
		if (x41_strcmp(argv[1], "init") == 0) return init(argv[2]);
		if (x41_strcmp(argv[1], "add") == 0 && argc == 4) return post(argv[2], argv[3]);
		if (x41_strcmp(argv[1], "get") == 0 && argc == 4) return get(argv[2], argv[3]);
		if (x41_strcmp(argv[1], "list") == 0) return get_entry(argv[2]);
}
