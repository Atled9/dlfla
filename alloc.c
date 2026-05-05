#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define BYTES 1024

typedef struct Header{
	struct Header *prev, *next;
	size_t block_size;
	uint8_t free;
} Header;

static uint8_t bytearr[BYTES];

static const uint8_t ALIGN = sizeof(void *);
static const uint8_t HEADER_SIZE = sizeof(Header);

static Header *begin;
static uint8_t init = 0;

void initFreelist(void) 
{
	assert(!init);

	begin = (Header *)bytearr;
	begin->prev = NULL;
	begin->block_size = BYTES - HEADER_SIZE;
	begin->free = 1;

	Header *end = (Header *)(bytearr + BYTES - HEADER_SIZE);
	end->next = NULL;
	end->block_size = HEADER_SIZE;
	end->free = 0;

	begin->next = end;
	end->prev = begin;
	init = 1;
}

void printblocks(void)
{
	assert(init);
	printf("Total Bytes: %zu, Header Size: %zu\n", BYTES, HEADER_SIZE);

	Header *curr = begin;
	while (curr) {
		printf("Free: %d, Block Size: %zu\n", curr->free, curr->block_size);
		curr = curr->next;
	}
}

void *alloc(size_t bytes)
{
	assert(init);

	size_t total_bytes = bytes + HEADER_SIZE;
	uint8_t mod = total_bytes % ALIGN;
	if (mod)
		total_bytes += ALIGN - mod;

	Header *curr = begin;
	while (curr && (!curr->free || total_bytes > curr->block_size))
		curr = curr->next;

	assert(curr);

	/* perfect or near-perfect fit: not enough leftover space for new header */
	if (curr->block_size - total_bytes < HEADER_SIZE) {
		curr->free = 0;
		return (uint8_t *)curr + HEADER_SIZE;
	}

	Header *newheader = (Header *)((uint8_t *)curr + total_bytes);
	newheader->prev = curr;
	newheader->next = curr->next;
	newheader->block_size = curr->block_size - total_bytes;
	newheader->free = 1;

	curr->next->prev = newheader;
	curr->next = newheader;
	curr->block_size = total_bytes;
	curr->free = 0;

	return (uint8_t *)curr + HEADER_SIZE;
}
void dealloc(void *ptr)
{
	assert(init && ptr);
	Header *block = (Header *)((uint8_t *)ptr - HEADER_SIZE);
	assert(!block->free);

	if (block->prev && block->prev->free && block->next && block->next->free) {
		block->prev->next = block->next->next;
		block->next->next->prev = block->prev;
		block->prev->block_size += block->block_size + block->next->block_size;
		return;
	}
	if (block->prev && block->prev->free) {
		block->prev->next = block->next;
		block->next->prev = block->prev;
		block->prev->block_size += block->block_size;
		return;
	}
	if (block->next && block->next->free) {
		block->block_size += block->next->block_size;
		block->next = block->next->next;
		block->next->prev = block->prev;
		block->free = 1;
		return;
	}
	block->free = 1;
	return;
}
