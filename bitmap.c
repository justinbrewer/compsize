#include "bitmap.h"

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

#define CHUNK_SHIFT 21 /* 2MiB */
#define WORD_SHIFT 5
#define WORD_MASK ((1 << WORD_SHIFT) - 1)
#define BIT_TO_CHUNK(_b) ((_b) >> CHUNK_SHIFT)
#define BIT_TO_WORD(_b) ((_b) >> WORD_SHIFT)
#define BIT_TO_MASK(_b) (1 << ((_b) & WORD_MASK))
#define MAP_SIZE(_s) ((_s) * (1 << CHUNK_SHIFT))

static int resize(struct bitmap *b, size_t new_size)
{
	if(new_size <= b->size)
		return 0;

	void *new_map = mremap(b->map, MAP_SIZE(b->size), MAP_SIZE(new_size),
			MREMAP_MAYMOVE);

	if(new_map == MAP_FAILED)
		return -ENOMEM;

	b->map = new_map;
	b->size = new_size;
	return 0;
}

int bitmap_init(struct bitmap *b)
{
	b->size = 1;
	b->map = mmap(NULL, MAP_SIZE(b->size), PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	return b->map == MAP_FAILED;
}

void bitmap_destroy(struct bitmap *b)
{
	munmap(b->map, MAP_SIZE(b->size));
}

/**
 * Mark and return previous value of a bit
 *
 * Resizes map if necessary. Returns -ENOMEM if resize fails.
 */
int bitmap_mark(struct bitmap *b, size_t bit)
{
	int rv;
	size_t chunk, word, mask;

	chunk = BIT_TO_CHUNK(bit);
	word = BIT_TO_WORD(bit);
	mask = BIT_TO_MASK(bit);

	rv = resize(b, chunk + 1);

	if(rv)
		return rv;

	if(b->map[word] & mask)
		return 1;

	b->map[word] |= mask;
	return 0;
}
