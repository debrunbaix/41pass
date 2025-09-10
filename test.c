#include	"../include/libc/x41_unistd.h"

typedef __UINT32_TYPE__  uint32_t;

int ask_pass(const char *filename, char **master_pass);
int save_vault(const char *filename, const char *master_pass);
int load_vault(const char *filename);
int load_vault_raw(const char *filename, const char *master_pass);
void xor_cipher(char *str, const char *key);

char	canary[] = "c4n4ryM4st3r";

typedef struct node {
		struct node *next;
		char entry[64];
		char password[64];
} node_t;

static node_t *head = NULL;

static void copy_bounded(char dest[], size_t cap, const char *src) {
		size_t i = 0;
		if (!src) { dest[0] = '\0'; return; }
		while (src[i] && i + 1 < cap) {
				dest[i] = src[i];
				i++;
		}
		dest[i] = '\0';
}


static size_t cstrnlen_(const char *s, size_t max) {
    size_t i = 0;
    if (!s) return 0;
    while (i < max && s[i]) i++;
    return i;
}

static void clear_list(node_t **headp) {
    node_t *cur = headp ? *headp : NULL;
    while (cur) {
        node_t *nx = cur->next;
        x41_free(cur);
        cur = nx;
    }
    if (headp) *headp = NULL;
}

static void append(node_t **headp, const char *entry, const char *password) {
    if (!headp) return;
    node_t *n = (node_t *)x41_malloc(sizeof *n);
    if (!n) return;
    copy_bounded(n->entry, sizeof n->entry, entry);
    copy_bounded(n->password, sizeof n->password, password);
    n->next = NULL;

    if (!*headp) { *headp = n; return; }
    node_t *cur = *headp;
    while (cur->next) cur = cur->next;
    cur->next = n;
}

int save_vault(const char *filename, const char *master_pass)
{
    // 1) Lire l'en-tête existant pour le réécrire tel quel
    int fd = x41_open(filename, 0, 0);            // O_RDONLY
    if (fd < 0) { x41_puts("open read failed"); return ERROR; }

    uint8_t L = 0;
    if (x41_read(fd, (char *)&L, 1) != 1) { x41_puts("read L failed"); x41_close(fd); return ERROR; }
    if (L == 0 || L > 63) { x41_puts("bad canary len"); x41_close(fd); return ERROR; }

    char enc_canary[64];
    if (x41_read(fd, enc_canary, L) != L) { x41_puts("read canary failed"); x41_close(fd); return ERROR; }
    x41_close(fd);

    // 2) Réécrire le fichier à neuf : en-tête + liste
    fd = x41_open(filename, 577, 0644);           // O_WRONLY|O_TRUNC|O_CREAT (comme ton init)
    if (fd < 0) { x41_puts("open write failed"); return ERROR; }

    if (x41_write(fd, (char *)&L, 1) != 1) { x41_puts("write L failed"); x41_close(fd); return ERROR; }
    if (x41_write(fd, enc_canary, L) != L) { x41_puts("write canary failed"); x41_close(fd); return ERROR; }

    // 3) Compter et écrire le nombre d'entrées
    uint32_t count = 0;
    for (node_t *p = head; p; p = p->next) count++;
    if (x41_write(fd, (char *)&count, sizeof count) != (ssize_t)sizeof count) {
        x41_puts("write count failed"); x41_close(fd); return ERROR;
    }

    // 4) Écrire chaque nœud (entry, password) sans '\0'
    for (node_t *p = head; p; p = p->next) {
		char encrypted_entry[64], encrypted_password[64];
        
        // Copier et chiffrer entry
        copy_bounded(encrypted_entry, sizeof encrypted_entry, p->entry);
        xor_cipher(encrypted_entry, master_pass);
        
        // Copier et chiffrer password
        copy_bounded(encrypted_password, sizeof encrypted_password, p->password);
        xor_cipher(encrypted_password, master_pass);

        uint8_t elen = (uint8_t)cstrnlen_(p->entry, sizeof p->entry);
        uint8_t plen = (uint8_t)cstrnlen_(p->password, sizeof p->password);

        if (x41_write(fd, (char *)&elen, 1) != 1) { x41_puts("write elen failed"); x41_close(fd); return ERROR; }
        if (x41_write(fd, encrypted_entry, elen) != elen) { x41_puts("write entry failed"); x41_close(fd); return ERROR; }

        if (x41_write(fd, (char *)&plen, 1) != 1) { x41_puts("write plen failed"); x41_close(fd); return ERROR; }
        if (x41_write(fd, encrypted_password, plen) != plen) { x41_puts("write password failed"); x41_close(fd); return ERROR; }
    }

    x41_close(fd);
    return SUCCESS;
}

int load_vault_raw(const char *filename, const char *master_pass)
{
    int fd = x41_open(filename, 0, 0);            // O_RDONLY
    if (fd < 0) { x41_puts("open read failed"); return ERROR; }

    uint8_t L = 0;
    if (x41_read(fd, (char *)&L, 1) != 1) { x41_puts("read L failed"); x41_close(fd); return ERROR; }
    if (L == 0 || L > 63) { x41_puts("bad canary len"); x41_close(fd); return ERROR; }

    char skip[64];
    if (x41_read(fd, skip, L) != L) { x41_puts("read canary failed"); x41_close(fd); return ERROR; }

    uint32_t count = 0;
    if (x41_read(fd, (char *)&count, sizeof count) != (ssize_t)sizeof count) {
        x41_puts("read count failed"); x41_close(fd); return ERROR;
    }

    clear_list(&head);

    for (uint32_t i = 0; i < count; i++) {
        uint8_t elen = 0, plen = 0;
        char entry[64], pwd[64];

        if (x41_read(fd, (char *)&elen, 1) != 1) { x41_puts("read elen failed"); x41_close(fd); return ERROR; }
        if (elen >= sizeof entry) elen = sizeof entry - 1;
        if (x41_read(fd, entry, elen) != elen) { x41_puts("read entry failed"); x41_close(fd); return ERROR; }
        entry[elen] = '\0';

        if (x41_read(fd, (char *)&plen, 1) != 1) { x41_puts("read plen failed"); x41_close(fd); return ERROR; }
        if (plen >= sizeof pwd) plen = sizeof pwd - 1;
        if (x41_read(fd, pwd, plen) != plen) { x41_puts("read password failed"); x41_close(fd); return ERROR; }
        pwd[plen] = '\0';

		xor_cipher(entry, master_pass);
        xor_cipher(pwd, master_pass);

        append(&head, entry, pwd);
    }

    x41_close(fd);
    return SUCCESS;
}

int load_vault(const char *filename)
{
		char *master_pass = NULL;
    if (ask_pass(filename, &master_pass) != SUCCESS) {
        x41_puts("Wrong password");
		if (master_pass) x41_free(master_pass);
        return ERROR;
    }
	int result = load_vault_raw(filename, master_pass);
    x41_free(master_pass);
    return result;
}

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
		uint32_t initial_count = 0;
		char canary_copy[64];

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
		copy_bounded(canary_copy, sizeof(canary_copy), canary);
		xor_cipher(canary_copy, line);
		x41_free(line);
		canary_len = sizeof(canary) - 1;

		fd = x41_open(filename, 577, 0644);
		if (fd < 0)
		{
				x41_puts("Failed to create vault");
				return 1;
		}

		x41_write(fd, &canary_len, 1);
		//x41_write(fd, canary, sizeof(canary));
		x41_write(fd, canary_copy, canary_len);
		x41_write(fd, (char *)&initial_count, sizeof(initial_count));
		x41_close(fd);

		x41_puts("Vault initialized.");
		return 0;
}

void	push(node_t **headp, const char *entry, const char *password)
{
		if (!headp) return;

		node_t *new_node = (node_t *)x41_malloc(sizeof *new_node);
		if (!new_node) return;

		copy_bounded(new_node->entry, sizeof new_node->entry, entry);
		copy_bounded(new_node->password, sizeof new_node->password, password);

		new_node->next = *headp;
		*headp = new_node;
}

int		ask_pass(const char *filename, char **master_pass)
{
		int		fd;
		uint8_t	len = 0;
		char	buffer[32];
		char	*pass = NULL;
		size_t	n = 0;

		fd = x41_open(filename, 0, 0);
		if (fd < 0)
		{
				x41_puts("Failed to x41_open file.");
				return ERROR;
		}
		if (x41_read(fd, (char *)&len, 1) != 1)
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
		if (pass) {
			size_t pass_len = cstrnlen_(pass, 1000);
			if (pass_len > 0 && pass[pass_len - 1] == '\n') {
				pass[pass_len - 1] = '\0';
			}
		}
		xor_cipher(buffer, pass);
		if (x41_strncmp(buffer, canary, len) != 0)
		{
				x41_close(fd);
				x41_free(pass);
				return ERROR;
		}
		x41_close(fd);
		if (master_pass) {
			*master_pass = pass;
		} else {
			x41_free(pass);
		}
		return SUCCESS;
}

int		post(const char	*filename, const char *entry)
{
		char *master_pass = NULL;
		if (ask_pass(filename, &master_pass) != SUCCESS) {
				x41_puts("Wrong password");
				return ERROR;
		}
		if (load_vault_raw(filename, master_pass) != SUCCESS) {
				x41_puts("could not load vault");
				return ERROR;
		}

		char *buffer = NULL;
		size_t n = 0;
		ssize_t len;

		x41_puts("Password to store: ");
		len = x41_getline(&buffer, &n, STDIN);
		if (len <= 0) {
				x41_puts("read error");
				x41_free(buffer);
				x41_free(master_pass);
				return ERROR;
		}
		if (buffer[len - 1] == '\n') buffer[len - 1] = '\0';

		push(&head, entry, buffer);
		save_vault(filename, master_pass);

		x41_free(buffer);
		x41_free(master_pass);
		return SUCCESS;
}

int		get(const char *filename, const char *entry)
{
		if (load_vault(filename) != SUCCESS) return ERROR;
		
		for (node_t *p = head; p; p = p->next)
		{
				if (x41_strcmp(p->entry, entry) == 0) {
						x41_puts("Entry: ");
						x41_puts(p->entry);
						x41_puts("Password: ");
						x41_puts(p->password);
						return SUCCESS;
				}
		}
		x41_puts("Entry not found");
		return ERROR;
}

int		get_entry(const char *filename)
{
		if (load_vault(filename) != SUCCESS) return ERROR;
		for (node_t *p = head; p; p = p->next)
		{
				x41_puts("Entry: ");
				x41_puts(p->entry);
				x41_puts("Password: ");
				x41_puts(p->password);
				x41_puts("---");
		}
		return SUCCESS;
}

int		truc(int argc, char **argv, char **envp)
{
		if (argc < 3) return 1;
		if (x41_strcmp(argv[1], "init") == 0) return init(argv[2]);
		if (x41_strcmp(argv[1], "add") == 0 && argc == 4) return post(argv[2], argv[3]);
		if (x41_strcmp(argv[1], "get") == 0 && argc == 4) return get(argv[2], argv[3]);
		if (x41_strcmp(argv[1], "list") == 0) return get_entry(argv[2]);
		return 0;
}
