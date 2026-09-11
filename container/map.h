#ifndef CONTAINER_MAP_H
# define CONTAINER_MAP_H

# include "memory/allocator.h"
# include "util/hash.h"

typedef struct map Map;

typedef bool (*MapCmpF)(Map*, const void*, const void*);
typedef Hash (*MapHashF)(Map*, const void*);

struct map {
	Allocator *allocator;
	MapHashF hashF;
	MapCmpF cmpF;
	u64 keySize;
	u64 valueSize;
	u64 used;
	u64 reserved;
	u8 prime;
	void *data;
	u8 spare : 1;
	bool rehashLocked;
};

Map *map_create(Allocator *allocator, u64 keySize, u64 valueSize, MapHashF hash_f, MapCmpF cmp_f);
bool map_insert(Map *map, const void *key, const void *value);
void *map_get(Map *map, const void *key);
void map_delete(Map *map, const void *key);
void map_destroy(Map *map);

Hash map_hash_fixxedStr(Map *map, const void *str);
bool map_cmp_fixxedStr(Map *map, const void *str1, const void *str2);

#endif // CONTAINER_MAP_H
