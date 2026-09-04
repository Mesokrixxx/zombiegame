#ifndef MEMORY_ALLOCATOR_H
# define MEMORY_ALLOCATOR_H

# include "util/types.h"

typedef struct allocator {
	void *(*allocImpl)(struct allocator*, u64);
	void (*freeImpl)(struct allocator*, void*);

	u64 used;
	u64 reserved;
} Allocator;

void *allocator_alloc(Allocator *alloc, u64 size);
void allocator_free(Allocator *alloc, void *ptr);

#endif // MEMORY_ALLOCATOR_H
