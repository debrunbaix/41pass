#ifndef PWMAN_H
#define PWMAN_H

#include "libc/x41_unistd.h"
#include "libc/types.h"

#define MAX_ENTRY_SIZE 64
#define MAX_PASSWORD_SIZE 64
#define CIPHER_KEY 42  // Clé simple pour le chiffrement

typedef struct node {
    struct node *next;
    char entry[MAX_ENTRY_SIZE];
    char password[MAX_PASSWORD_SIZE];
} node_t;

// Fonctions de chiffrement
void simple_encrypt(char *data, int len);
void simple_decrypt(char *data, int len);

// Fonctions de gestion de la base de données
node_t *create_node(const char *entry, const char *password);
void free_list(node_t *head);
node_t *load_database(const char *filename, const char *master_password);
int save_database(node_t *head, const char *filename);

// Fonctions principales
int init(const char *filename);
int post(const char *filename, const char *entry);
int get(const char *filename, const char *entry);
int get_entry(const char *filename);

// Utilitaires
int verify_master_password(node_t *head, const char *master_password);
char *prompt_password(const char *prompt);

#endif
