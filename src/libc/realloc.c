#include "../include/libc/allocator.h"

static	void byte_copy(void *dst, const void *src, size_t n)
{
		unsigned char *d = (unsigned char *)dst;
		const unsigned char *s = (const unsigned char *)src;
		for (size_t i = 0; i < n; ++i) d[i] = s[i];
}

static	void split_block_local(struct block_header *b, size_t req_aligned)
{
		size_t total = b->size;

		if (total >= req_aligned + MIN_SPLIT_SIZE)
		{
				char *base = (char *)b;
				struct block_header *n =
				(struct block_header *)(base + sizeof(struct block_header) + req_aligned);

				n->size = total - req_aligned - sizeof(struct block_header);
				n->free = 1;
				n->prev = b;
				n->next = b->next;

				if (n->next) n->next->prev = n; else g_heap_tail = n;

				b->size = req_aligned;
				b->next = n;
		}
}

static	int try_expand_in_place(struct block_header *b, size_t req_aligned)
{
		if (!b->next || !b->next->free) return 0;

		size_t combined = b->size + sizeof(struct block_header) + b->next->size;
		if (combined < req_aligned) return 0;

		struct block_header *n = b->next;
		b->size = combined;
		b->next = n->next;
		if (b->next) b->next->prev = b; else g_heap_tail = b;

		split_block_local(b, req_aligned);
		return 1;
}

void	*realloc(void *ptr, size_t size)
{
		if (!ptr) return malloc(size);
		if (size == 0)
		{ 
				free(ptr); 
				return 0;
		}

		size_t req = ALIGN_UP(size);
		struct block_header *b = payload_to_block(ptr);

		if (b->size >= req)
		{
				split_block_local(b, req);
				return ptr;
		}

		if (try_expand_in_place(b, req)) return ptr;

		void *newp = malloc(size);
		if (!newp) return 0;

		size_t to_copy = (b->size < req) ? b->size : req;
		byte_copy(newp, ptr, to_copy);

		free(ptr);
		return newp;
}

