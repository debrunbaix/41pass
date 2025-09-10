#include "../include/pwman.h"

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
