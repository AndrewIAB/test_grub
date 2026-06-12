#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "multiboot.h"

#define MAX(X, Y) ((X) < (Y) ? (Y) : (X))

#define PAGE_SIZE ((uint32_t)4096)
// (1<<32) - PAGE_SIZE
#define MAX_PAGE_ADDR ((uint32_t)0x3FFFFFFFFF000)

extern void* _kbin_beg;
extern void* _kbin_end;

extern uint8_t* pgmap_bfree;
extern uint8_t* pgmap_breserved;
void* pgmap_alloc();
void pgmap_free(void*);
void pgmap_reserve(void*);

void* memset (void*, register int, register size_t);

static bool in_free_region(void* page_base, multiboot_memory_map_t* mmap_table, size_t mmap_table_len) {
	for (size_t i = 0; i < mmap_table_len;) {
		multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)((uint32_t)mmap_table + i);
		if (
				mmap->type != MULTIBOOT_MEMORY_AVAILABLE ||
				mmap->addr < (uint32_t)page_base ||
				(uint32_t)page_base + PAGE_SIZE > mmap->addr + mmap->len)
		{
			i += mmap->size + 4;
			continue;
		}
		return true;
	}
	return false;
}

static bool in_os_space(void* page_base) {
	return
		page_base < _kbin_beg &&
		page_base + PAGE_SIZE <= _kbin_beg &&
		page_base > _kbin_end;
}

static bool in_grub_region(void* page_base, multiboot_info_t* multiboot_info) {
	if (
			(uint32_t)page_base >= (uint32_t)multiboot_info &&
			(uint32_t)page_base < (uint32_t)multiboot_info + sizeof(multiboot_info_t))
	{
		return true;
	}
	if (
			(uint32_t)page_base >= multiboot_info->mmap_addr &&
			(uint32_t)page_base < multiboot_info->mmap_addr + multiboot_info->mmap_length)
	{
		return true;
	}
	return false;
}

static void reserve_region(multiboot_memory_map_t* mmap) {
	for (uint32_t i = mmap->addr; i < mmap->addr + mmap->len; i += PAGE_SIZE) {
		pgmap_reserve((void*)i);
	}
}

static void set_reserved_pages(
		multiboot_info_t* multiboot_info,
		multiboot_memory_map_t* mmap_table,
		void* max_page) {
	size_t mmap_table_len = multiboot_info->mmap_length;

	// Reserve all pages above max address
	for (uint32_t i = (uint32_t)max_page; i < MAX_PAGE_ADDR; i += PAGE_SIZE) {
		pgmap_reserve((void*)i);
	}

	// Reserve OS data
	for (uint32_t i = (uint32_t)_kbin_beg; i < (uint32_t)_kbin_end; i += PAGE_SIZE) {
		pgmap_reserve((void*)i);
	}

	// Reserve MMAP regions
	for (size_t i = 0; i < mmap_table_len;) {
		multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)((uint32_t)mmap_table + i);
		if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) {
			reserve_region(mmap);
		}

		i += mmap->size + 4;
	}

	// Reserve mmap table
	pgmap_reserve(mmap_table);

	// Reserve multiboot info
	pgmap_reserve(multiboot_info);

}

void pgmap_init(multiboot_info_t* multiboot_info) {
	multiboot_memory_map_t* mmap_table = (void*)multiboot_info->mmap_addr;
	size_t mmap_table_len = multiboot_info->mmap_length;

	uint8_t* max_page = NULL;

	for (size_t i = 0; i < mmap_table_len;) {
		multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)((uint32_t)mmap_table + i);
		uint8_t* mmap_max_addr = (uint8_t*)((uint32_t)mmap->addr + (uint32_t)mmap->len);
		max_page = MAX(max_page, mmap_max_addr);

		i += mmap->size + 4;
	}

	pgmap_bfree = (void*)~0;

	// Find free regions to place pagemap pages
	for (uint32_t i = 0; i < (uint32_t)max_page; i += PAGE_SIZE) {
		if (
				(!in_free_region((void*)i, mmap_table, mmap_table_len)) ||
				in_os_space((void*)i) ||
				in_grub_region((void*)i, multiboot_info))
		{
			continue;
		}

		if (pgmap_bfree == (void*)~0) {
			pgmap_bfree = (void*)i;
		}
		else {
			pgmap_breserved = (void*)i;
			break;
		}
	}

	memset(pgmap_bfree, ~0, PAGE_SIZE);
	memset(pgmap_breserved, 0, PAGE_SIZE);

	set_reserved_pages(multiboot_info, mmap_table, max_page);
}
