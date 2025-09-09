#include "../include/libc/x41_unistd.h"
#include "../include/libc/allocator.h"

struct block_header *g_heap_head = 0;
struct block_header *g_heap_tail = 0;

static	void split_block(struct block_header *b, size_t req_aligned)
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

static	struct block_header *find_free_block(size_t req_aligned)
{
		for (struct block_header *cur = g_heap_head; cur; cur = cur->next)
		{
				if (cur->free && cur->size >= req_aligned)
				{
						split_block(cur, req_aligned);
						cur->free = 0;
						return cur;
				}
		}
		return 0;
}

static	struct block_header *request_space(size_t req_aligned)
{
		size_t total = sizeof(struct block_header) + req_aligned;

		if (total > (size_t)0x7fffffff) return 0;

		void *mem = x41_sbrk((int)total);
		if (mem == (void *)-1) return 0;

		struct block_header *b = (struct block_header *)mem;
		b->size = req_aligned;
		b->free = 0;
		b->prev = g_heap_tail;
		b->next = 0;

		if (g_heap_tail) g_heap_tail->next = b; else g_heap_head = b;
		g_heap_tail = b;

		return b;
}

void	*x41_malloc(size_t size)
{
		if (size == 0) return 0;

		size_t req = ALIGN_UP(size);

		struct block_header *b = find_free_block(req);
		if (!b)
		{
				b = request_space(req);
				if (!b) return 0;
		}
		return block_to_payload(b);
}

