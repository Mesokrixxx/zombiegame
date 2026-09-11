#ifndef CONTAINER_SPARSESET_H
# define CONTAINER_SPARSESET_H

# include "container/dynlist.h"

typedef struct {
	Allocator *allocator;
	Dynlist(u64) sparse;
	void *dense;
	u64 reserved;
	u64 used;
	u64 dataSize;
} SparseSet;

SparseSet *sparseset_create(Allocator *allocator, u64 dataSize, u64 reserve);
void sparseset_insert(SparseSet *ss, u64 id, const void *data);
void *sparseset_get(SparseSet *ss, u64 id);
void sparseset_delete(SparseSet *ss, u64 id);
void sparseset_destroy(SparseSet *ss);

#endif // CONTAINER_SPARSESET_H
