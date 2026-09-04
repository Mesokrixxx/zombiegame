#include "mallocator.h"
#include <malloc.h>

static void *allocImpl(Allocator *alloc, u64 size) {
	void *ptr = malloc(size);

	if (ptr) {
		size = malloc_usable_size(ptr);
		alloc->used += size;
		alloc->reserved += size;
	}
	return ptr;
}

static void freeImpl(Allocator *alloc, void *ptr) {
	u64 size = malloc_usable_size(ptr);

	alloc->used -= size;
	alloc->reserved -= size;
	free(ptr);
}

void mallocator_init(Mallocator *mallocator) {
	*mallocator = (Mallocator){
		.allocImpl = allocImpl,
		.freeImpl = freeImpl
	};
}
