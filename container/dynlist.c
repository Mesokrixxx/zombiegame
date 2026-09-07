#include "dynlist.h"
#include "math/util.h"
#include <string.h>

u64 dynlist_size(void *list) {
	return _dynlist_getHeader(list)->used;
}

u64 dynlist_capacity(void *list) {
	return _dynlist_getHeader(list)->reserved;
}

void dynlist_clear(void *list) {
	_dynlist_getHeader(list)->used = 0;
}

void dynlist_destroy(void *list) {
	if (!list)
		return ;

	DynlistHeader *header = _dynlist_getHeader(list);

	allocator_free(header->allocator, header);
}

u64 _dynlist_headerSize() {
	return math_roundupPow2(sizeof(DynlistHeader), 2);
}

DynlistHeader *_dynlist_getHeader(void *list) {
	return (DynlistHeader *)((u8 *)list - _dynlist_headerSize());
}

void _dynlist_initImpl(Allocator *allocator, void **list, u64 typeSize, u64 reserve) {
	reserve = math_roundupPow2(reserve, 2);

	DynlistHeader *header = allocator_alloc(allocator, _dynlist_headerSize() + typeSize * reserve);
	*header = (DynlistHeader){
		.allocator = allocator,
		.typeSize = typeSize,
		.reserved = reserve,
	};

	*list = (u8 *)header + _dynlist_headerSize();
}

bool _dynlist_reserveImpl(void **list, u64 reserve) {
	DynlistHeader *oHeader = _dynlist_getHeader(*list);

	u64 nCap = oHeader->reserved;
	if (nCap >= reserve)
		return false;

	while (nCap < reserve)
		nCap *= 2;

	DynlistHeader *nHeader = 
		allocator_alloc(oHeader->allocator, _dynlist_headerSize() + oHeader->typeSize * nCap); 
	*nHeader = (DynlistHeader){
		.allocator = oHeader->allocator,
		.typeSize = oHeader->typeSize,
		.reserved = nCap,
		.used = oHeader->used,
	};

	memcpy(
		(u8 *)nHeader + _dynlist_headerSize(), 
		(u8 *)oHeader + _dynlist_headerSize(), 
		oHeader->used * oHeader->typeSize);	
	allocator_free(oHeader->allocator, oHeader);

	*list = (u8 *)nHeader + _dynlist_headerSize();
	return true;
}

void _dynlist_copyImpl(void **dest, const void *src) {
	DynlistHeader *srcHeader = _dynlist_getHeader((void *)src);

	if (*dest)
		_dynlist_reserveImpl(dest, srcHeader->used);
	else
	 	_dynlist_initImpl(srcHeader->allocator, dest, srcHeader->typeSize, srcHeader->used);
	memcpy(*dest, src, srcHeader->used * srcHeader->typeSize);
	_dynlist_getHeader(*dest)->used = srcHeader->used;
}

void _dynlist_pushBackNImpl(void **list, const void *x, u64 n) {
	u64 size = dynlist_size(*list);

	_dynlist_reserveImpl(list, size + n);

	DynlistHeader *header = _dynlist_getHeader(*list);

	for (u64 i = 0; i < n; i++)
		memcpy((u8 *)*list + (size + i) * header->typeSize, x, header->typeSize);
	header->used += n;
}

void _dynlist_pushBackArrayImpl(void **list, const void *arr, u64 n) {
	u64 size = dynlist_size(*list);

	_dynlist_reserveImpl(list, size + n);
	
	DynlistHeader *header = _dynlist_getHeader(*list);

	memcpy((u8 *)*list + size * header->typeSize, arr, header->typeSize * n);
	header->used += n;
}
