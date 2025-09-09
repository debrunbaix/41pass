#ifndef LIBC_ALLOCATOR_H
#define LIBC_ALLOCATOR_H

/*
** Petit allocateur user-space basé sur sbrk().
** On maintient une liste doublement chaînée de blocs (allocés ou libres).
** Chaque bloc commence par un "header" suivi de la zone utilisateur (payload).
**
**  [block_header][........ payload aligné ........]
**
** Alignement : 16 octets (x86_64 friendly).
*/

#include "types.h"   /* size_t, uintptr_t, etc. */
#include "unistd.h"  /* sbrk() */

#define ALIGNMENT     16UL
#define ALIGN_UP(n)   (((n) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

/* Taille minimale pour pouvoir scinder un bloc proprement :
   header + au moins ALIGNMENT octets de payload */
#define MIN_SPLIT_SIZE  (sizeof(struct block_header) + ALIGNMENT)

/* En-tête de chaque bloc. Taille typique : 32 octets (alignée naturellement). */
struct block_header {
    size_t size;                    /* Taille utile (payload) en octets, alignée */
    int    free;                    /* 1 si libre, 0 si occupé */
    struct block_header *next;      /* Bloc suivant dans l’ordre mémoire */
    struct block_header *prev;      /* Bloc précédent dans l’ordre mémoire */
};

/* Pointeurs sentinelles du tas de l’allocateur */
extern struct block_header *g_heap_head;
extern struct block_header *g_heap_tail;

/* Helpers de conversion */
static inline void *block_to_payload(struct block_header *b) {
    return (void *)((char *)b + sizeof(struct block_header));
}

static inline struct block_header *payload_to_block(void *p) {
    return (struct block_header *)((char *)p - sizeof(struct block_header));
}

#endif /* LIBC_ALLOCATOR_H */
