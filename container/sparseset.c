#include "sparseset.h"
#include "math/util.h"
#include <string.h>

const u64 EMPTY_SLOT = UINT64_MAX;

static u64 denseSize(u64 capacity) {
	return math_roundupPow2(sizeof(u64) * capacity, MAX_ALIGN);
}

static u64 denseTotSize(u64 dataSize, u64 capacity) {
	return denseSize(capacity) + math_roundupPow2(dataSize * capacity, MAX_ALIGN);
}

static u32 *getDense(SparseSet *ss) {
	return ss->dense;
}

static u8 *getData(SparseSet *ss) {
	return (u8 *)ss->dense + denseSize(ss->reserved);
}

SparseSet *sparseset_create(Allocator *allocator, u64 dataSize, u64 reserve) {
	SparseSet *ss = allocator_alloc(allocator, sizeof(SparseSet));
	void *dense = allocator_alloc(allocator, denseTotSize(dataSize, reserve));
	*ss = (SparseSet) {
		.allocator = allocator,
		.dense = dense,
		.reserved = reserve,
		.dataSize = dataSize,
	};

	dynlist_init(allocator, ss->sparse, reserve);
	dynlist_pushBackN(ss->sparse, &EMPTY_SLOT, reserve);
	return ss;
}

void sparseset_insert(SparseSet *ss, u64 id, const void *data) {
	if (dynlist_reserve(ss->sparse, id + 1))
		dynlist_pushBackN(ss->sparse, &EMPTY_SLOT, dynlist_capacity(ss->sparse) - dynlist_size(ss->sparse));

	u64 idx = ss->sparse[id];
	if (idx == EMPTY_SLOT) {
		u32 *dense = getDense(ss);

		idx = ss->used++;
		dense[idx] = id;
		ss->sparse[id] = idx;
	}

	memcpy(getData(ss) + idx * ss->dataSize, data, ss->dataSize);
}

void *sparseset_get(SparseSet *ss, u64 id) {
	u64 idx = ss->sparse[id];
	
	return idx != EMPTY_SLOT ? ((u8 *)ss->dense + denseSize(ss->reserved) + ss->dataSize * idx) : NULL;
}

void sparseset_destroy(SparseSet *ss) {
	if (!ss)
		return ;
	dynlist_destroy(ss->sparse);
	allocator_free(ss->allocator, ss->dense);
	allocator_free(ss->allocator, ss);
}
