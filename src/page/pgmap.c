#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

#include "../multiboot.h"

#define MAX_PAGES (1 << 20)
#define PAGE_BYTEBIT(PAGE)\
	uint32_t byte = (PAGE) / 8, bit = (PAGE) % 8
#define PAGE_ADDR(PAGE)\
	(void*)((PAGE) << 12)
#define PAGE_INDEX(ADDR)\
	((uint32_t)(ADDR) >> 12)

uint8_t* pgmap_bfree;
uint8_t* pgmap_breserved;

typedef uint32_t pgindex_t;

bool pgmap_is_free(pgindex_t page) {
	PAGE_BYTEBIT(page);
	return (pgmap_bfree[byte] & (1 << bit)) != 0;
}

bool pgmap_is_reserved(pgindex_t page) {
	PAGE_BYTEBIT(page);
	return (pgmap_breserved[byte] & (1 << bit)) != 0;
}

void pgmap_set_free(pgindex_t page, bool val) {
	PAGE_BYTEBIT(page);
	pgmap_bfree[byte] = val ?
		pgmap_bfree[byte] | (1 << bit) :
		pgmap_bfree[byte] & (~(1 << bit));
}

void pgmap_set_reserved(pgindex_t page, bool val) {
	PAGE_BYTEBIT(page);
	pgmap_breserved[byte] = val ?
		pgmap_breserved[byte] | (1 << bit) :
		pgmap_breserved[byte] & (~(1 << bit));
}

pgindex_t pgmap_get_free() {
	for (pgindex_t i = 0; i < MAX_PAGES; i++) {
		if (pgmap_is_free(i)) {
			return i;
		}
	}
	/* Return impossible page */
	return ~0;
}

void* pgmap_alloc() {
	pgindex_t idx = pgmap_get_free();
	pgmap_set_free(idx, false);
	return PAGE_ADDR(idx);
}

void pgmap_reserve(void* page) {
	pgindex_t idx = PAGE_INDEX(page);
	pgmap_set_reserved(idx, true);
	/* Allows kernel to reserve pages that were not allocated with pgmap_alloc, mainly to allocate itself and grub-loaded tables */
	pgmap_set_free(idx, false);
}

void pgmap_free(void* page) {
	pgindex_t idx = PAGE_INDEX(page);
	pgmap_set_free(idx, true);
	/* Freeing a reserved page also revokes reserved flag */
	pgmap_set_reserved(idx, false);
}
