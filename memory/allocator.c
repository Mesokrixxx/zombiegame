#include "allocator.h"
#include "util/assert.h"
#include "util/error.h"

void *allocator_alloc(Allocator *alloc, u64 size) {
	void *ptr = alloc->allocImpl(alloc, size);
	
	ASSERT(ptr, "failed to allocate %zu bytes: %s", size, error_msgGet());
	return ptr;
}

void allocator_free(Allocator *alloc, void *ptr) {
	if (!ptr)
		return ;
	alloc->freeImpl(alloc, ptr);
}
