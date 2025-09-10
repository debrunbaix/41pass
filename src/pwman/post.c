#include "../include/pwman.h"

/**
 * Ajouter une nouvelle entrée dans la base de données
 */
int post(const char *filename, const char *entry)
{
		node_t *head, *current, *new_node;
		char *master_password, *new_password;
		char encrypted_password[MAX_PASSWORD_SIZE];

		// Demander le master password
		master_password = prompt_password("Master password: ");
		if (!master_password)
		{
				x41_puts("Erreur: impossible de lire le master password");
				return ERROR;
		}

		// Charger la base de données
		head = load_database(filename, master_password);
		if (!head) return ERROR;  // L'erreur a déjà été affichée

		// Vérifier si l'entrée existe déjà
		current = head->next;  // Skip le nœud MASTER
		while (current)
		{
				if (x41_strcmp(current->entry, entry) == 0)
				{
						x41_printf("L'entrée '%s' existe déjà\n", entry);
						free_list(head);
						return ERROR;
				}
				current = current->next;
		}

		// Demander le nouveau mot de passe
		x41_printf("Password pour '%s': ", entry);
		new_password = prompt_password("");
		if (!new_password)
		{
				x41_puts("Erreur: impossible de lire le password");
				free_list(head);
				return ERROR;
		}

		// Chiffrer le mot de passe
		x41_strcpy(encrypted_password, new_password);
		simple_encrypt(encrypted_password, MAX_PASSWORD_SIZE);

		// Créer le nouveau nœud
		new_node = create_node(entry, encrypted_password);
		if (!new_node)
		{
				x41_puts("Erreur: impossible de créer la nouvelle entrée");
				free_list(head);
				return ERROR;
		}

		// Ajouter à la fin de la liste
		current = head;
		while (current->next) current = current->next;
		current->next = new_node;

		// Sauvegarder
		if (save_database(head, filename) != SUCCESS)
		{
				free_list(head);
				return ERROR;
		}

		x41_printf("Entrée '%s' ajoutée avec succès\n", entry);
		free_list(head);
		return SUCCESS;
}
