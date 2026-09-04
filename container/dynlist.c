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

void _dynlist_reserveImpl(void **list, u64 reserve) {
	DynlistHeader *oHeader = _dynlist_getHeader(*list);

	u64 nCap = oHeader->reserved;
	if (nCap >= reserve)
		return ;

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
}

void _dynlist_pushBackImpl(void **list, void *x) {
	u64 size = dynlist_size(*list);

	_dynlist_reserveImpl(list, size + 1);	
	
	DynlistHeader *header = _dynlist_getHeader(*list);

	memcpy((u8 *)*list + size * header->typeSize, x, header->typeSize);
	header->used++;
}

void *_dynlist_getImpl(void *list, u64 idx) {
	return (u8*)list + _dynlist_getHeader(list)->typeSize * idx;
}
