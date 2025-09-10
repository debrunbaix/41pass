#include "../include/pwman.h"
#include "../include/libc/colors.h"

int delete_entry(struct data *data, const char *entry){
	node_t *current = data->head->next; // Skip le nœud MASTER
	node_t *prev = NULL;
	
	// Chercher l'entrée
	while (current) {
		if (x41_strcmp(current->entry, entry) == 0) {
			// Trouvé, supprimer le nœud
			if (prev) {
				prev->next = current->next;
			} else {
				// Suppression du nœud head (MASTER)
				data->head = current->next;
			}
			x41_bzero(current, sizeof(node_t));
			x41_free(current);
			x41_printf("Entrée '"COLOR_MAGENTA"%s"COLOR_RESET"' supprimée avec succès\n", entry);
			return SUCCESS;
		}
		prev = current;
		current = current->next;
	}
	x41_printf("Entrée '"COLOR_MAGENTA"%s"COLOR_RESET"' non trouvée\n", entry);
	return ERROR;
}

int add_entry(struct data *data, const char *entry, const char *password){
	char encrypted_password[MAX_PASSWORD_SIZE];
	
	
	// Vérifier si l'entrée existe déjà
	node_t *current = data->head->next;  // Skip le nœud MASTER
	while (current) {
		if (x41_strcmp(current->entry, entry) == 0) {
			x41_printf("L'entrée '"COLOR_MAGENTA"%s"COLOR_RESET"' existe déjà\n", entry);
			return ERROR;
		}
		current = current->next;
	}

	// Chiffrer le mot de passe
	x41_strcpy(encrypted_password, password);
	simple_encrypt(encrypted_password, MAX_PASSWORD_SIZE);
	// Ajouter la nouvelle entrée au début de la liste (après MASTER)
	node_t *new_node = create_node(entry, password);
	if (!new_node) {
		x41_puts("Erreur: impossible de créer la nouvelle entrée");
		return ERROR;
	}
	
	new_node->next = data->head->next;
	data->head->next = new_node;
	return SUCCESS;
}

node_t *find_entry(struct data *data, const char *entry){
	// Chercher l'entrée
    node_t *current = data->head->next;  // Skip le nœud MASTER
    char decrypted_password[MAX_PASSWORD_SIZE];
	
	while (current) {
        if (x41_strcmp(current->entry, entry) == 0) {
            // Déchiffrer et afficher le mot de passe
            x41_strcpy(decrypted_password, current->password);
            simple_decrypt(decrypted_password, MAX_PASSWORD_SIZE);
            
            x41_printf("Password pour '"COLOR_MAGENTA"%s"COLOR_RESET"': %s\n", entry, decrypted_password);
            return current;
        }
        current = current->next;
    }
	x41_printf("Entrée '"COLOR_MAGENTA"%s"COLOR_RESET"' non trouvée\n", entry);
	return NULL;
}

/**
 * Récupérer un mot de passe spécifique
 */
int get(const char *filename, const char *entry)
{
    node_t *head, *current;
    char *master_password;
    char decrypted_password[MAX_PASSWORD_SIZE];
    
    // Demander le master password
    master_password = prompt_password("Master password: ");
    if (!master_password) {
        x41_puts("Erreur: impossible de lire le master password");
        return ERROR;
    }
    
    // Charger la base de données
    head = load_database(filename, master_password);
    if (!head) {
        return ERROR;
    }
    
    // Chercher l'entrée
    current = head->next;  // Skip le nœud MASTER
    while (current) {
        if (x41_strcmp(current->entry, entry) == 0) {
            // Déchiffrer et afficher le mot de passe
            x41_strcpy(decrypted_password, current->password);
            simple_decrypt(decrypted_password, MAX_PASSWORD_SIZE);
            
            x41_printf("Password pour '%s': %s\n", entry, decrypted_password);
            free_list(head);
            return SUCCESS;
        }
        current = current->next;
    }
    
    x41_printf("Entrée '%s' non trouvée\n", entry);
    free_list(head);
    return ERROR;
}

/**
 * Lister toutes les entrées (sans les mots de passe)
 */
int get_entry(const char *filename)
{
		node_t *head, *current;
		char *master_password;
		int count = 0;

		// Demander le master password
		master_password = prompt_password("Master password: ");
		if (!master_password)
		{
				x41_puts("Erreur: impossible de lire le master password");
				return ERROR;
		}

		// Charger la base de données
		head = load_database(filename, master_password);
		if (!head) return ERROR;

		x41_puts("Entrées disponibles:");
		x41_puts("------------------");

		// Lister toutes les entrées (sauf MASTER)
		current = head->next;
		while (current)
		{
				x41_printf("- %s\n", current->entry);
				count++;
				current = current->next;
		}

		if (count == 0)
		{
				x41_puts("Aucune entrée trouvée");
		} else {
				x41_printf("\nTotal: %d entrée(s)\n", count);
		}

		free_list(head);
		return SUCCESS;
}
