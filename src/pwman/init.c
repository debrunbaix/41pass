#include "../include/pwman.h"
#include "../include/libc/x41_unistd.h"
#include "../include/libc/x41_errno.h"

/**
 * Créer un nouveau nœud
 */
node_t	*create_node(const char *entry, const char *password)
{
		node_t *node = x41_malloc(sizeof(node_t));
		if (!node) return NULL;

		node->next = NULL;
		x41_strcpy(node->entry, (char *)entry);
		x41_strcpy(node->password, (char *)password);

		return node;
}

/**
 * Libérer toute la liste chaînée
 */
void	free_list(node_t *head)
{
		node_t *next;

		while (head)
		{
				next = head->next;
				x41_bzero(head, sizeof(node_t));
				x41_free(head);
				head = next;
		}
}

/**
 * Demander un mot de passe de façon sécurisée
 */
char	*prompt_password(const char *prompt)
{
		static char password[MAX_PASSWORD_SIZE];
		char *line = NULL;
		size_t len = 0;

		x41_printf("%s", prompt);

		if (x41_getline(&line, &len, STDIN) > 0)
		{
				// Supprimer le '\n' final
				int i = 0;
				while (line[i] && line[i] != '\n' && i < MAX_PASSWORD_SIZE - 1)
				{
						password[i] = line[i];
						i++;
				}
				password[i] = '\0';

				if (line) x41_free(line);
				return password;
		}

		if (line) x41_free(line);
		return NULL;
}

/**
 * Initialiser une nouvelle base de données
 */
int		init(const char *filename)
{
		node_t *head;
		char *master_password;
		char encrypted_master[MAX_PASSWORD_SIZE];

		// Demander le master password
		master_password = prompt_password("Entrez le master password: ");
		if (!master_password)
		{
				x41_puts("Erreur: impossible de lire le master password");
				return ERROR;
		}

		// Créer le premier nœud avec le master password chiffré par lui-même
		x41_strcpy(encrypted_master, master_password);
		simple_encrypt(encrypted_master, MAX_PASSWORD_SIZE);

		head = create_node("MASTER", encrypted_master);
		if (!head)
		{
				x41_puts("Erreur: impossible de créer la base de données");
				return ERROR;
		}

		// Sauvegarder dans le fichier
		if (save_database(head, filename) != SUCCESS)
		{
				x41_puts("Erreur: impossible de sauvegarder la base de données");
				free_list(head);
				return ERROR;
		}

		x41_printf("Base de données '%s' initialisée avec succès\n", filename);
		free_list(head);
		return SUCCESS;
}
