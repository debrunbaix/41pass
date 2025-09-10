#include "pwman.h"
#include "../include/libc/colors.h"

#define PROMPT ">> "

void print_all_entries(struct data *data) {
    node_t *current = data->head->next;  // Skip le nœud MASTER
    char decrypted_password[MAX_PASSWORD_SIZE];

    x41_printf("Liste des entrées dans le coffre-fort:\n");
    x41_printf("=====================================:\n");
    if (!current) {
        x41_printf(" (aucune entrée)\n");
        x41_printf("=====================================:\n");
        return;
    }
    while (current) {
        x41_strcpy(decrypted_password, current->password);
        simple_decrypt(decrypted_password, MAX_PASSWORD_SIZE);
        x41_printf(COLOR_MAGENTA " - %s\n" COLOR_RESET, current->entry);
        current = current->next;
    }
    x41_printf("=====================================:\n");
}

int set_master_password(struct data *data, const char *new_master) {
    char encrypted_master[MAX_PASSWORD_SIZE];
    node_t *current = data->head;
    
    // Chiffrer le nouveau master password
    x41_strcpy(encrypted_master, new_master);
    simple_encrypt(encrypted_master, MAX_PASSWORD_SIZE);
    
    if (!current || x41_strcmp(current->entry, "MASTER") != 0){
        // Créer le nœud MASTER s'il n'existe pas
        data->head = create_node("MASTER", encrypted_master);
        if (!data->head) return ERROR;
    } else {
        // Mettre à jour le nœud MASTER existant
        x41_strcpy(current->password, encrypted_master);
    }
    return SUCCESS;
}

int connexion(struct data *data) {    
    // Demander le nom du fichier de la base de données
    while(1) {
        x41_printf("Entrez le nom du fichier du coffre-fort ou '"COLOR_CYAN"quit"COLOR_RESET"' pour quitter :\n(default:"COLOR_CYAN" vault.db" COLOR_RESET ")  ");
        data->filename = x41_readline(PROMPT);
        if (!data->filename) {
            x41_printf("Erreur de lecture du nom de fichier. Sortie.\n");
            continue;
        } else {
            // Trimining newline character
            data->filename[x41_strcspn(data->filename, "\n")] = '\0';
        }
        if (x41_strcmp(data->filename, "quit") == 0) {
            x41_printf("Aurevoir!\n");
            return 0;
        }
        if (data->filename[0] == '\0') {
            data->filename = x41_strdup("vault.db");
        }
        
        break;
    }
    
    // Demander le master password
    x41_printf("Entrez le master password: ");
    data->master_password = x41_readline(PROMPT);
    if (!data->master_password) {
        x41_puts("Erreur: impossible de lire le master password");
        if (data->filename) x41_free(data->filename);
        data->filename = NULL;
        return ERROR;
    }
    // Trimining newline character
    data->master_password[x41_strcspn(data->master_password, "\n")] = '\0';

    // Vérifier si le fichier existe
    if (!file_exists(data->filename)) {
        x41_printf("Le fichier '" COLOR_CYAN "%s" COLOR_RESET "' n'existe pas. Initialisation d'un nouveau coffre-fort.\n", data->filename);
        if (set_master_password(data, data->master_password) == ERROR) {
            x41_puts("Erreur: impossible de créer le nœud MASTER");
            x41_free(data->filename);
            data->filename = NULL;
            x41_free(data->master_password);
            data->master_password = NULL;
            return ERROR;
        }
    }
    else {
        // Charger la base de données avec vérification
        data->head = load_database(data->filename, data->master_password);
    }
    if (!data->head) {
            x41_free(data->filename);
            data->filename = NULL;
            x41_free(data->master_password);
            data->master_password = NULL;
            return ERROR;
        }
    return 1;
}

int help(void) {
    x41_printf("Usage : pwman <commandes> <args>\n");
    x41_printf("Commandes:\n");
    x41_printf("  init <nom de fichier>          Initialiser un nouveau coffre-fort de mots de passe\n");
    x41_printf("  add <nom de fichier> <entrée>  Ajouter une nouvelle entrée de mot de passe\n");
    x41_printf("  get <nom de fichier> <entrée>  Récupérer une entrée de mot de passe\n");
    x41_printf("  list <nom de fichier>          Lister toutes les entrées dans le coffre-fort\n");
    x41_printf("\nSi aucune commande n'est donnée, démarre en mode interactif.\n");
    return 0;
}

// Simple interactive REPL for pwman
int interactive_REPL(struct data *data) {
    
    node_t *current = NULL;
    int errsv = 0;
    char *command;
    char *entry;
    char *password;

    x41_printf("\n==================  " COLOR_BLUE "PWMAN" COLOR_RESET"  ==================\n");
    x41_printf("Bienvenue sur Password Manager!\n");
    x41_printf("Tapez " COLOR_CYAN "'help'" COLOR_RESET " pour une liste des commandes.\n\n");

    if((errsv = connexion(data)) < 1) return errsv;

    while (1) {

        x41_printf("Entrez une commande (" COLOR_CYAN "add" COLOR_RESET ", " COLOR_CYAN "get" COLOR_RESET ", " COLOR_CYAN "list" COLOR_RESET ", " COLOR_CYAN "delete" COLOR_RESET ", "COLOR_CYAN "change" COLOR_RESET ", "COLOR_CYAN "quit" COLOR_RESET "):\n");
        command = x41_readline(PROMPT);
        

        if (!command) {
            x41_printf("Erreur de lecture de la commande. Sortie.\n");
            continue;
        }else {
           // Trimining newline character
            command[x41_strcspn(command, "\n")] = '\0'; 
        }
        if (x41_strcmp(command, "add") == 0) {
            x41_printf("Ajout d'une nouvelle entrée :\n");
            entry = x41_readline(PROMPT);     
            x41_printf("Entrez le mot de passe :\n");
            password = x41_readline(PROMPT);
            if (!entry || !password) {
                x41_printf("Le nom de l'entrée et le mot de passe ne peuvent pas être vides.\n");
                x41_free(entry);
                x41_free(password);
                continue;
            }
            // Trimining newline character
            
            password[x41_strcspn(password, "\n")] = '\0';
            entry[x41_strcspn(entry, "\n")] = '\0';

            int res = add_entry(data, entry, password);
            if (res == ERROR) {
                x41_printf("Échec de l'ajout de l'entrée.\n");
                
            } else {
                x41_printf("Entrée ajoutée avec succès.\n");
            }
            
            
        } else if (x41_strcmp(command, "delete") == 0) {
            x41_printf("Entrée de mot de passe à supprimer : ");
            entry = x41_readline(PROMPT);
            if (!entry) {
                x41_printf("L'entrée de mot de passe ne peut pas être vide.\n");
                continue;
            }
            // Trimining newline character
            entry[x41_strcspn(entry, "\n")] = '\0';

            // Supprimer l'entrée
            delete_entry(data, entry);
            x41_free(entry);
            
        } else if (x41_strcmp(command, "get") == 0) {
            x41_printf("Entrée de mot de passe à récupérer : ");
            entry = x41_readline(PROMPT);
            if (!entry) {
                x41_printf("L'entrée de mot de passe ne peut pas être vide.\n");
                continue;
            }
            // Trimining newline character
            entry[x41_strcspn(entry, "\n")] = '\0';
            current = find_entry(data, entry);

            if (!current) {
                x41_printf("Échec de la récupération de l'entrée.\n");
                x41_free(entry);
                continue;
            }
            x41_printf("Voulez-vous changer le mot de passe? (oui/non) :\n");
            command = x41_readline(PROMPT);
            if (!command) {
                x41_printf("Erreur de lecture de la commande. Sortie.\n");
                x41_free(entry);
                continue;
            }
            // Trimining newline character
            command[x41_strcspn(command, "\n")] = '\0';
            if (x41_strcmp(command, "oui") == 0) {
                x41_printf("Entrez le nouveau mot de passe: ");
                password = x41_readline(PROMPT);
                // Trimining newline character
                password[x41_strcspn(password, "\n")] = '\0';
                if (!password) {
                    x41_printf("Le mot de passe ne peut pas être vide.\n");
                    x41_free(entry);
                    x41_free(password);
                    continue;
                }
                x41_strcpy(current->password, password);
                x41_printf("Mot de passe mis à jour avec succès.\n");
                x41_free(password);
            }
        } else if (x41_strcmp(command, "list") == 0) {
            print_all_entries(data);
        } else if (x41_strcmp(command, "help") == 0) {
            x41_printf("Commande disponible:\n");
            x41_printf("  add   - Ajouter une nouvelle entrée de mot de passe\n");
            x41_printf("  get   - Récupérer une entrée de mot de passe\n");
            x41_printf("  delete  - Supprimer une entrée de mot de passe\n");
            x41_printf("  list  - Lister toutes les entrées\n");
            x41_printf("  change  - Changer le master password\n");
            x41_printf("  help  - Afficher ce message d'aide\n");
            x41_printf("  quit  - Quitter le programme\n");
        }else if (x41_strcmp(command, "change") == 0) {
            x41_printf("Entrez le nouveau master password: ");
            entry = x41_readline(PROMPT);
            if (!entry) {
                x41_printf("Le master password ne peut pas être vide.\n");
                x41_free(entry);
                continue;
            }
            //trimining newline character
            entry[x41_strcspn(entry, "\n")] = '\0';
            // Mettre à jour le master password
            if (set_master_password(data, entry) == ERROR) {
                x41_printf("Échec de la mise à jour du master password.\n");
                x41_free(entry);
                continue;
            }
            x41_printf("Master password mis à jour avec succès.\n");
            x41_free(entry);
        }
        else if (x41_strcmp(command, "") == 0) {
            continue;
        } else if (x41_strcmp(command, "quit") == 0) {
            x41_printf("Aurevoir!\n");
            return SUCCESS;
        } else {
            x41_printf("Commande inconnue: %s\n", command);
        }
    }
    return ERROR; // Should never reach here
}
