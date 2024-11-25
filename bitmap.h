#ifndef COMPSIZE__BITMAP_H__
#define COMPSIZE__BITMAP_H__

#include <stddef.h>
#include <stdint.h>

/**
 * Simple pseudo-sparse bitmap
 *
 * Storage is mmaped to take advantage of lazy page allocation. Automatically
 * resizes when accessing bits past the end of the current storage.
 *
 * Worst case memory usage 32MiB per TiB of 4K filesystem extents.
 *
 * @map: Word storage of bitmap
 * @size: Size of map, as number of 2MiB chunks
 */
struct bitmap
{
    uint32_t *map;
    size_t size;
};

int bitmap_init(struct bitmap *b);
void bitmap_destroy(struct bitmap *b);
int bitmap_mark(struct bitmap *b, size_t bit);

#endif
