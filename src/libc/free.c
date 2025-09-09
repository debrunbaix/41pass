#include "../include/libc/allocator.h"

void	merge_with_next(struct block_header *b)
{
		if (b && b->next && b->next->free)
		{
				struct block_header *n = b->next;
				b->size += sizeof(struct block_header) + n->size;
				b->next = n->next;
				if (b->next) b->next->prev = b;
				else g_heap_tail = b;
		}
}

void	x41_free(void *ptr)
{
		if (!ptr) return;

		struct block_header *b = payload_to_block(ptr);
		b->free = 1;

		merge_with_next(b);
		if (b->prev && b->prev->free) merge_with_next(b->prev);
}

