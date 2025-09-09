#include	"../include/libc/x41_unistd.h"

char	canary[] = "c4n4ryM4st3r";

/**
 * Function to xor a string with a key (32b max).
 *
 * str: String to crypt.
 * key: String that permit to xor str.
 */
void	xor_cipher(char *str, const char *key)
{
		size_t i = 0;
		for (i = 0; str[i] != '\0'; i++) str[i] ^= key[i % 32];
}

/**
 * Function to create db file and init the password to x41_open it.
 *
 * filename: Name of the db file.
 *
 * info:
 *		Use of XOR to compare the result of password ^ canary with what is in the file.
 */
int		init(const char	*filename)
{
		int		fd;
		char	*line = NULL;
		size_t	n = 0;
		ssize_t	len;
		uint8_t	canary_len;

		x41_puts("Master password: ");

		len = x41_getline(&line, &n, STDIN);
		if (len <= 0)
		{
				x41_puts("Error x41_reading password");
				return 1;
		}
		if (len -1 < 12)
		{
				x41_puts("Password too short");
				return 1;
		}

		if (line[len - 1] == '\n') line[len - 1] = '\0';
		xor_cipher(canary, line);
		x41_free(line);
		canary_len = sizeof(canary) - 1;

		fd = x41_open(filename, 577, 0644);
		if (fd < 0)
		{
				x41_puts("Failed to create vault");
				return 1;
		}

		x41_write(fd, &canary_len, 1);
		x41_write(fd, canary, sizeof(canary));
		x41_close(fd);

		x41_puts("Vault initialized.");
		return 0;
}

int		ask_pass(const char *filename)
{
		int		fd;
		uint8_t	len = 0;
		char	buffer[32];
		char	*pass = NULL;
		size_t	n = 0;

		x41_printf("DEBUG: canary[] %s\n", canary);

		fd = x41_open(filename, 0, 0);
		if (fd < 0)
		{
				x41_puts("Failed to x41_open file.");
				return ERROR;
		}
		if (x41_read(fd, &len, 1) != 1)
		{
				x41_puts("failed to x41_read canary len.");
				return ERROR;
		}
		if (x41_read(fd, buffer, len) != len)
		{
				x41_puts("Failed to x41_read canary.");
				x41_close(fd);
				return ERROR;
		}
		buffer[len] = '\0';
		x41_getline(&pass, &n, STDIN);
		x41_printf("DEBUG: buffer[] %s\n", buffer);
		x41_printf("DEBUG: pass %s\n", pass);
		xor_cipher(buffer, pass);
		x41_printf("DEBUG: buffer[] after %s\n", buffer);
		if (x41_strncmp(buffer, canary, len) != 0)
		{
				x41_puts("Wrong password");
				x41_close(fd);
				return ERROR;
		}
		x41_puts("Good pass");
		x41_close(fd);
		return SUCCESS;
}

int		post(const char	*filename, const char *entry)
{
		ask_pass(filename);
		return 0;
}

int		get(const char *filename, const char *entry)
{
		return 0;
}

int		get_entry(const char *filename)
{
		return 0;
}

int		main(int argc, char **argv, char **envp)
{
		if (argc < 3) return 1;
		if (x41_strcmp(argv[1], "init") == 0) return init(argv[2]);
		if (x41_strcmp(argv[1], "add") == 0 && argc == 4) return post(argv[2], argv[3]);
		if (x41_strcmp(argv[1], "get") == 0 && argc == 4) return get(argv[2], argv[3]);
		if (x41_strcmp(argv[1], "list") == 0) return get_entry(argv[2]);
		return 0;
}
