#ifndef ECS_H
# define ECS_H

# include "memory/allocator.h"
# include "container/bitset.h"
# include "container/sparseset.h"
# include "util/macros.h"

typedef u64 ECSComponentID;
typedef u64 ECSEntity;

typedef struct {
	Bitset compBitset;
	u64 reserved;
	u64 used;
} ECSArchetype;

typedef struct {
	Allocator *allocator;
	Bitset tmpBitset;
	SparseSet *entities;
	Dynlist(u64) freeIds;
	Dynlist(u64) components;
	Dynlist(ECSArchetype*) archetypes;
} ECS;

ECS *ecs_create(Allocator *allocator);
ECSComponentID ecs_registerComponent(ECS *ecs, u64 dataSize);
ECSEntity ecs_newEntity(ECS *ecs, ECSComponentID *cmps, u64 cmpCount);
void *ecs_getComponent(ECS *ecs, ECSArchetype *arch, ECSComponentID component);
void *ecs_get(ECS *ecs, ECSEntity entity, ECSComponentID component);
void ecs_deleteEntity(ECS *ecs, ECSEntity entity);
void ecs_destroy(ECS *ecs);

# define ecs_forEach(ecs, cmps, it) \
	_ecs_forEachImpl(ecs, cmps, ARRAY_SIZE(cmps), it, CONCAT(_ecs, __COUNTER__), CONCAT(_it, __COUNTER__))

# define _ecs_forEachImpl(ecs, cmps, cmpCount, it, ecsname, itname) \
	typedef struct { \
		ECSArchetype *archetype; \
		u64 entityCount; \
		u64 idx, end; \
	} itname; \
	ECS *ecsname = (ecs); \
	for (itname it = ({ \
		u64 idx = _ecs_getArchetypeIdx(ecsname, cmps, cmpCount); \
		u64 end = dynlist_size(ecsname->archetypes); \
		(itname){ \
			.archetype = idx != end ? ecsname->archetypes[idx] : NULL, \
			.entityCount = idx != end ? ecsname->archetypes[idx]->used : 0, \
			.idx = idx, \
			.end = end, \
		}; }); \
	it.idx < it.end; \
	it.idx = _ecs_getNextArchetypeIdx(ecsname, it.idx),  \
	it.archetype = it.idx != it.end ? ecs->archetypes[it.idx] : NULL, \
	it.entityCount = it.idx != it.end ? it.archetype->used : 0)

u64 _ecs_getArchetypeIdx(ECS *, ECSComponentID *, u64);
u64 _ecs_getNextArchetypeIdx(ECS *, u64);

#endif // ECS_H
