#include "../include/pwman.h"

#define O_RDONLY 0
#define O_WRONLY 1
#define O_CREAT  64
#define O_TRUNC  512

/**
 * Charger la base de données depuis un fichier
 */
node_t *load_database(const char *filename, const char *master_password)
{
    int fd;
    node_t *head = NULL;
    node_t *current = NULL;
    node_t temp_node;
    char master_check[MAX_PASSWORD_SIZE];
    
    fd = x41_open(filename, O_RDONLY, 0644);
    if (fd < 0) {
        x41_puts("Erreur: impossible d'ouvrir le fichier");
        return NULL;
    }
    
    // Lire le premier nœud (master password)
    if (x41_read(fd, (char *)&temp_node, sizeof(node_t)) != sizeof(node_t)) {
        x41_puts("Erreur: fichier corrompu");
        x41_close(fd);
        return NULL;
    }
    
    // Vérifier le master password
    x41_strcpy(master_check, temp_node.password);
    simple_decrypt(master_check, MAX_PASSWORD_SIZE);
    
    if (x41_strcmp(master_check, master_password) != 0) {
        x41_puts("Erreur: master password incorrect");
        x41_close(fd);
        return NULL;
    }
    
    // Créer la tête de liste
    head = create_node(temp_node.entry, temp_node.password);
    if (!head) {
        x41_close(fd);
        return NULL;
    }
    current = head;
    
    // Lire les autres nœuds
    while (x41_read(fd, (char *)&temp_node, sizeof(node_t)) == sizeof(node_t)) {
        current->next = create_node(temp_node.entry, temp_node.password);
        if (!current->next) {
            free_list(head);
            x41_close(fd);
            return NULL;
        }
        current = current->next;
    }
    
    x41_close(fd);
    return head;
}

/**
 * Sauvegarder la base de données dans un fichier
 */
int save_database(node_t *head, const char *filename)
{
    int fd;
    node_t *current = head;
    
    fd = x41_open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) {
        x41_puts("Erreur: impossible de créer/ouvrir le fichier");
        return ERROR;
    }
    
    // Écrire tous les nœuds
    while (current) {
        if (x41_write(fd, current, sizeof(node_t)) != sizeof(node_t)) {
            x41_puts("Erreur: impossible d'écrire dans le fichier");
            x41_close(fd);
            return ERROR;
        }
        current = current->next;
    }
    
    x41_close(fd);
    return SUCCESS;
}

/**
 * Vérifier le master password
 */
int verify_master_password(node_t *head, const char *master_password)
{
    char decrypted_master[MAX_PASSWORD_SIZE];
    
    if (!head || x41_strcmp(head->entry, "MASTER") != 0) {
        return ERROR;
    }
    
    x41_strcpy(decrypted_master, head->password);
    simple_decrypt(decrypted_master, MAX_PASSWORD_SIZE);
    
    return x41_strcmp(decrypted_master, master_password) == 0 ? SUCCESS : ERROR;
}
