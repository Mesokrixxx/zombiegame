#ifndef CONTAINER_DYNLIST_H
# define CONTAINER_DYNLIST_H

# include "memory/allocator.h"
# include "util/macros.h"

# define Dynlist(t) t *

typedef struct {
	Allocator *allocator;
	u64 typeSize;
	u64 used;
	u64 reserved;
} DynlistHeader;

u64 dynlist_size(void *list);
u64 dynlist_capacity(void *list);
void dynlist_clear(void *list);
void dynlist_destroy(void *list);

# define dynlist_init(alloc, list, reserve) \
	_dynlist_initImpl(alloc, (void**)&(list), sizeof(*(list)), reserve)

# define dynlist_reserve(list, reserve) \
	_dynlist_reserveImpl((void **)&(list), reserve)

# define dynlist_pushBack(list, x) \
	_dynlist_pushBackImpl((void **)&(list), (x))

# define dynlist_forEach(list, it) \
	_dynlist_forEachImpl(list, it, CONCAT(dll, __COUNTER__), CONCAT(dli, __COUNTER__))

# define dynlist_get(list, idx) \
	((typeof(list))_dynlist_getImpl(list, idx))

DynlistHeader *_dynlist_getHeader(void *list);
u64 _dynlist_headerSize();

void _dynlist_initImpl(Allocator *, void **, u64, u64);
void _dynlist_reserveImpl(void **, u64);
void _dynlist_pushBackImpl(void **, void *);
void *_dynlist_getImpl(void *, u64);

# define _dynlist_forEachImpl(list, it, listname, itname) \
	typedef struct { \
		typeof(list) elem; \
		u64 idx, end; \
	} itname; \
	typeof(list) *listname = &(list); \
	for (itname it = { \
		.elem = *listname, \
		.end = dynlist_size(*listname), \
	}; \
	it.idx < it.end; \
	it.idx++, it.elem = *listname + it.idx)

#endif // CONTAINER_DYNLIST_H
