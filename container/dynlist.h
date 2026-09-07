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
	_dynlist_pushBackNImpl((void **)&(list), (x), 1)

# define dynlist_pushBackN(list, x, n) \
	_dynlist_pushBackNImpl((void **)&(list), (x), (n))

# define dynlist_pushBackArray(list, arr, n) \
	_dynlist_pushBackArrayImpl((void **)&(list), (arr), (n))

# define dynlist_copy(dest, src) \
	_dynlist_copyImpl((void **)&(dest), (src))

# define dynlist_pop(list) \
	({ typeof(list) _list = (list); (typeof(_list))_list[--_dynlist_getHeader(_list)->used]; })

# define dynlist_forEach(list, it) \
	_dynlist_forEachOffsetImpl(list, 0, it, CONCAT(_dlist, __COUNTER__), CONCAT(_it, __COUNTER__))
	
# define dynlist_forEachOffset(list, off, it) \
	_dynlist_forEachOffsetImpl(list, (off), it, CONCAT(_dlist, __COUNTER__), CONCAT(_it, __COUNTER__))

DynlistHeader *_dynlist_getHeader(void *list);
u64 _dynlist_headerSize();

void _dynlist_initImpl(Allocator *, void **, u64, u64);
bool _dynlist_reserveImpl(void **, u64);
void _dynlist_copyImpl(void **, const void *);
void _dynlist_pushBackNImpl(void **, const void *, u64);
void _dynlist_pushBackArrayImpl(void **, const void *, u64);

# define _dynlist_forEachOffsetImpl(list, off, it, listname, itname) \
		typedef struct { \
		typeof(list) elem; \
		u64 idx, end; \
	} itname; \
	typeof(list) listname = (list); \
	for (itname it = { \
		.elem = listname + off, \
		.idx = off, \
		.end = dynlist_size(listname), \
	}; \
	it.idx < it.end; \
	it.idx++, it.elem = listname + it.idx)

#endif // CONTAINER_DYNLIST_H
