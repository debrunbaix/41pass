#ifndef PWMAN_H
#define PWMAN_H

#include "libc/x41_unistd.h"
#include "libc/x41_errno.h"
#include "libc/types.h"

#define MAX_ENTRY_SIZE 64
#define MAX_PASSWORD_SIZE 64
#define CIPHER_KEY 42  // Clé simple pour le chiffrement

typedef struct node {
    struct node *next;
    char entry[MAX_ENTRY_SIZE];
    char password[MAX_PASSWORD_SIZE];
} node_t;

extern int x41_errno;  // Déclaré dans errno.c

struct data {
    char *master_password;
    char *filename;
    node_t *head;
};

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
int add_entry(struct data *data, const char *entry, const char *password);
node_t *find_entry(struct data *data, const char *entry);
int delete_entry(struct data *data, const char *entry);
int file_exists(const char *filename);

// REPL interactive
int interactive_REPL(struct data *data);
int help(void);
int connexion(struct data *data);

#endif
