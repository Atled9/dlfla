#ifndef ALLOCATOR
#define ALLOCATOR

void initFreelist(void);

void printblocks(void);

void *alloc(size_t bytes); 
void dealloc(void *ptr);

#endif
