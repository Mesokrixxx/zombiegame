#include "ecs.h"
#include "math/util.h"
#include <string.h>

typedef struct {
	u64 archetype;
	u64 row;
} EntityLocation;

#define ARCHETYPE_MIN_ENTITY_COUNT 32

static void makeBitset(Bitset *bset, ECSComponentID *cmps, u64 cmpCount) {
	bitset_clear(*bset);
	for (u64 i = 0; i < cmpCount; i++)
		bitset_set(*bset, cmps[i], true);
}

static u8 *archetypeData(ECSArchetype *arch) {
	return (u8 *)arch + math_roundupPow2(sizeof(ECSArchetype), MAX_ALIGN);
}

ECS *ecs_create(Allocator *allocator) {
	ECS *ecs = allocator_alloc(allocator, sizeof(ECS));
	Bitset tmpBitset = bitset_create(allocator);
	SparseSet *ss = sparseset_create(allocator, sizeof(EntityLocation), 64);
	*ecs = (ECS){
		.allocator = allocator,
		.tmpBitset = tmpBitset,
		.entities = ss,
	};

	dynlist_init(allocator, ecs->freeIds, 32);
	dynlist_init(allocator, ecs->components, 32);
	dynlist_init(allocator, ecs->archetypes, 16);
	return ecs;
}

ECSComponentID ecs_registerComponent(ECS *ecs, u64 dataSize) {
	u64 listSize = dynlist_size(ecs->components);
	
	dynlist_pushBack(ecs->components, &dataSize);
	return listSize;
}

ECSEntity ecs_newEntity(ECS *ecs, ECSComponentID *cmps, u64 cmpCount) {
	ECSArchetype **archetypePtr = NULL;
	u64 archetypeIdx;

	makeBitset(&ecs->tmpBitset, cmps, cmpCount);
	dynlist_forEach(ecs->archetypes, it) {
		if (!bitset_eq((*it.elem)->compBitset, ecs->tmpBitset))
			continue ;
		archetypePtr = it.elem;
		archetypeIdx = it.idx;
		break ;
	}

	if (!archetypePtr) {
		u64 dataSize = 0;
		for (u64 i = 0; i < cmpCount; i++)
			dataSize += math_roundupPow2(ecs->components[cmps[i]] * ARCHETYPE_MIN_ENTITY_COUNT, MAX_ALIGN);
	
		ECSArchetype *nArchetype = allocator_alloc(ecs->allocator, math_roundupPow2(sizeof(ECSArchetype), MAX_ALIGN) + dataSize);
		*nArchetype = (ECSArchetype){
			.reserved = ARCHETYPE_MIN_ENTITY_COUNT,
		};

		bitset_copy(nArchetype->compBitset, ecs->tmpBitset);
		dynlist_pushBack(ecs->archetypes, &nArchetype);
		archetypeIdx = dynlist_size(ecs->archetypes) - 1;
		archetypePtr = &ecs->archetypes[archetypeIdx];
	}

	else if ((*archetypePtr)->used >= (*archetypePtr)->reserved) {
		u64 nCap = (*archetypePtr)->reserved * 2;
		
		u64 dataSize = 0;
		for (u64 i = 0; i < cmpCount; i++)
			dataSize += math_roundupPow2(ecs->components[cmps[i]] * nCap, MAX_ALIGN);

		ECSArchetype *nArchetype = allocator_alloc(ecs->allocator, math_roundupPow2(sizeof(ECSArchetype), MAX_ALIGN) + dataSize);
		*nArchetype = (ECSArchetype){
			.compBitset = (*archetypePtr)->compBitset,
			.reserved = nCap,
			.used = (*archetypePtr)->used,
		};

		u8 *nData = archetypeData(nArchetype);
		u8 *oData = archetypeData(*archetypePtr);
		for (u64 i = 0; i < cmpCount; i++) {
			u64 compSize = ecs->components[cmps[i]];

			memcpy(nData, oData, (*archetypePtr)->used * compSize);
			nData += math_roundupPow2(nCap * compSize, MAX_ALIGN);
			oData += math_roundupPow2((*archetypePtr)->reserved * compSize, MAX_ALIGN);
		}
		
		allocator_free(ecs->allocator, *archetypePtr);
		*archetypePtr = nArchetype;
	}
	
	EntityLocation loc = {
		.archetype = archetypeIdx,
		.row = (*archetypePtr)->used++,
	};

	u64 id;
	u64 freeIdsSize = dynlist_size(ecs->freeIds);
	if (freeIdsSize)
		id = *dynlist_pop(ecs->freeIds);
	else
		id = ecs->entities->used;
	sparseset_insert(ecs->entities, id, &loc);
	return id;
}

void *ecs_getComponent(ECS *ecs, ECSArchetype *arch, ECSComponentID component) {
	if (!bitset_bit(arch->compBitset, component))
		return NULL;

	u8 *data = archetypeData(arch);
	dynlist_forEach(ecs->components, it) {
		if (it.idx == component)
			break ;
		if (!bitset_bit(arch->compBitset, it.idx))
			continue ;
		data += math_roundupPow2(*it.elem * arch->reserved, MAX_ALIGN);
	}
	return data;
}

void *ecs_get(ECS *ecs, ECSEntity entity, ECSComponentID component) {
	EntityLocation *loc = sparseset_get(ecs->entities, entity);
	if (!loc)
		return NULL;

	u8 *cmpData = ecs_getComponent(ecs, ecs->archetypes[loc->archetype], component);

	return cmpData ? cmpData + ecs->components[component] * loc->row : NULL;
}

void ecs_deleteEntity(ECS *ecs, ECSEntity entity) {
	EntityLocation *loc = sparseset_get(ecs->entities, entity);
	if (!loc)
		return ;

	ECSArchetype *arch = ecs->archetypes[loc->archetype];

	u64 lastIdx = arch->used - 1;
	if (loc->row == lastIdx) {
		u8 *data = archetypeData(arch);
		dynlist_forEach(ecs->components, it) {
			u64 cmpSize = *it.elem;

			if (bitset_bit(arch->compBitset, it.idx)) {
				memcpy(
					data + loc->row * cmpSize, 
					data + lastIdx * cmpSize, 
					cmpSize);
			}
			data += math_roundupPow2(cmpSize * arch->reserved, MAX_ALIGN);
		}
	}

	arch->used--;
	sparseset_delete(ecs->entities, entity);
}

void ecs_destroy(ECS *ecs) {
	if (!ecs)
		return ;
	dynlist_destroy(ecs->freeIds);
	dynlist_forEach(ecs->archetypes, it) {
		bitset_destroy((*it.elem)->compBitset);
		allocator_free(ecs->allocator, *it.elem);
	}
	dynlist_destroy(ecs->archetypes);
	dynlist_destroy(ecs->components);
	sparseset_destroy(ecs->entities);
	bitset_destroy(ecs->tmpBitset);
	allocator_free(ecs->allocator, ecs);
}

u64 _ecs_getArchetypeIdx(ECS *ecs, ECSComponentID *cmps, u64 cmpCount) {
	makeBitset(&ecs->tmpBitset, cmps, cmpCount);
	dynlist_forEach(ecs->archetypes, it) {
		if (bitset_has((*it.elem)->compBitset, ecs->tmpBitset))
			return it.idx;
	}
	return dynlist_size(ecs->archetypes);
}

u64 _ecs_getNextArchetypeIdx(ECS *ecs, u64 off) {
	ECSArchetype *arch = ecs->archetypes[off];

	dynlist_forEachOffset(ecs->archetypes, off + 1, it) {
		if (bitset_has((*it.elem)->compBitset, arch->compBitset))
			return it.idx;
	}
	return dynlist_size(ecs->archetypes);
}
