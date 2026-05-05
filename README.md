## Doubly Linked Free List Allocator

### Usage Instructions

* `initFreelist()` must be called before any allocations can be made by the program
* Use `void *alloc(size_t size)` to request a block of memory in bytes: `int *arr = (int *)alloc(16 * sizeof(int));`
* Use `void dealloc(void *ptr)` to free a block of memory starting at `ptr`: `dealloc(arr);`
* A `printblocks()` function is available for debugging and scoping the byte array

### Program Details

* This program allocates memory on a single static byte array
* The size of the global array can be set in `alloc.c` at the `BYTES` macro
* Block alignment is set to the given system's pointer size `sizeof(void \*)`
* Allocations are made in linear time (O(n)) on a first-fit basis
* De-allocation will coalesce adjacent free blocks of memory in constant time by probing `block->prev` and `block->next`
* Extra space is assigned at the beginning of every block for a Header struct. `alloc()` will return the byte address directly after the header
* Extra space may also be assigned to a block on the basis of alignment
* If there is no room left over for a free Header when pulling from a free block, `alloc()` will return the entire free block
* As with any allocation scheme PLEASE DO NOT WRITE PAST or BEFORE the requested block. This will break the allocator 3:

* This program initializes the list with two blocks: 
	* a free block `begin` containing the total free memory
	* a permanently non-free block `end` containing only a header - used to assist with list traversal and probing.
