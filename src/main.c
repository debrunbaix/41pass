#include	"../include/libc/x41_unistd.h"
#include	"pwman.h"

void	clean_user_data(struct data *data)
{
		if (!data) return;
		if (data->master_password) {
			size_t len = x41_strlen(data->master_password);
			x41_bzero(data->master_password, len);
			x41_free(data->master_password);
			data->master_password = NULL;
		}
		if (data->filename) {
			x41_bzero(data->filename, x41_strlen(data->filename));
			x41_free(data->filename);
			data->filename = NULL;
		}
		free_list(data->head);
		data->head = NULL;
}

int		main(int argc, char **argv)
{
		struct data data = {0};
		if (argc == 1) {
			if (interactive_REPL(&data) == SUCCESS) {
				save_database(data.head, data.filename);
			}
			clean_user_data(&data);
			return (SUCCESS);
		}
		if (argc < 3) {
				help();
				return (ERROR);
		}
		if (x41_strcmp(argv[1], "init") == 0) return init(argv[2]);
		if (x41_strcmp(argv[1], "add") == 0 && argc == 4) return post(argv[2], argv[3]);
		if (x41_strcmp(argv[1], "get") == 0 && argc == 4) return get(argv[2], argv[3]);
		if (x41_strcmp(argv[1], "list") == 0) return get_entry(argv[2]);
}
