#include "map.h"
#include "math/util.h"
#include "util/macros.h"
#include "util/error.h"
#include <string.h>

typedef struct {
	u16 dist : 16;
	u16 hash : 15;
	bool used : 1;
} MapEntry;

typedef enum : u8 {
	REHASH_NONE = 0,
	REHASH_SHRINK = 1 << 0,
	REHASH_GROW = 1 << 1,
	REHASH_FORCED = 1 << 2,
} RehashMode;

typedef enum : u8 {
	REHASH_OK,
	REHASH_HAPPENED,
	REHASH_ERROR,
} RehashReturn;

const u32 PRIMES[] = {
    11, 53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
    196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189, 805306457, 1610612741
};

#define LOAD_HIGH 0.85
#define LOAD_LOW 0.15

#define MAX_DISTANCE UINT16_MAX
#define INVALID_POS (PRIMES[ARRAY_SIZE(PRIMES) - 1] + 1)

#define HASH_MASK ((1 << 15) - 1)

static void *map_alloc(Allocator *allocator, u64 capacity, u64 keySize, u64 valueSize) {
	u64 entrySize = math_roundupPow2(sizeof(MapEntry) * capacity, MAX_ALIGN);
	u64 dataSize = entrySize
		+ math_roundupPow2(keySize * capacity, MAX_ALIGN)
		+ math_roundupPow2(valueSize * capacity, MAX_ALIGN)
		+ math_roundupPow2(keySize * 2, MAX_ALIGN)
		+ math_roundupPow2(valueSize * 2, MAX_ALIGN);
	
	void *data = allocator_alloc(allocator, dataSize);
	
	memset(data, 0, entrySize);
	return data;
}

Map *map_create(Allocator *allocator, u64 keySize, u64 valueSize, map_hash_f hash_f, map_cmp_f cmp_f) {
	Map *map = allocator_alloc(allocator, sizeof(Map));
	void *data = map_alloc(allocator, PRIMES[0], keySize, valueSize);
	*map = (Map){
		.allocator = allocator,
		.hash_f = hash_f,
		.cmp_f = cmp_f,
		.keySize = keySize,
		.valueSize = valueSize,
		.reserved = PRIMES[0],
		.data = data,
	};

	return map;
}

static MapEntry *map_entries(Map *map) {
	return map->data;
}

static void *map_keys(Map *map) {
	return (u8 *)map_entries(map) + math_roundupPow2(sizeof(MapEntry) * map->reserved, MAX_ALIGN);
}

static void *map_values(Map *map) {
	return (u8 *)map_keys(map) + math_roundupPow2(map->keySize * map->reserved, MAX_ALIGN);
}

static RehashReturn map_rehash(Map *map, RehashMode rehash) {
	if (map->rehashLocked)
		return REHASH_OK;

	f32 load = map->used / (f32)map->reserved;
	bool shrink = load <= LOAD_LOW && rehash & REHASH_SHRINK;
	bool grow = load >= LOAD_HIGH && rehash & REHASH_GROW;
	bool forced = rehash & REHASH_FORCED;

	if (!grow && !shrink)
		return REHASH_OK;

	if ((grow && map->prime == ARRAY_SIZE(PRIMES) - 1) || (shrink && !map->prime)) {
		if (forced) {
			error_msgSet("canno't force shrink/grow map");
			return REHASH_ERROR;
		}
		return REHASH_OK;
	}

	if (shrink)
		map->prime--;
	else
		map->prime++;

	u64 oCap = map->reserved;
	void *oData = map->data;
	MapEntry *oEntries = map_entries(map);
	void *oKeys = map_keys(map);
	void *oValues = map_values(map);

	map->reserved = PRIMES[map->prime];
	map->used = 0;
	map->data = map_alloc(map->allocator, map->reserved, map->keySize, map->valueSize);
	
	map->rehashLocked = true;
	for (u64 i = 0; i < oCap; i++) {
		if (!oEntries[i].used)
			continue ;
		void *key = (u8 *)oKeys + map->keySize * i;
		void *value = (u8 *)oValues + map->valueSize * i;

		if (!map_insert(map, key, value))
			return REHASH_ERROR;
	}
	map->rehashLocked = false;

	allocator_free(map->allocator, oData);
	return REHASH_HAPPENED;
}

static void *map_keyAt(Map *map, u64 idx) {
	return (u8 *)map_keys(map) + map->keySize * idx;
}

static void *map_valueAt(Map *map, u64 idx) {
	return (u8 *)map_values(map) + map->valueSize * idx;
}

static void *map_spares(Map *map) {
	return (u8 *)map_values(map) + math_roundupPow2(map->valueSize * map->reserved, MAX_ALIGN);
}

static void *map_spareKey(Map *map) {
	return (u8 *)map_spares(map) + map->keySize * map->spare;
}

static void *map_spareValue(Map *map) {
	return (u8 *)map_spares(map)+ math_roundupPow2(map->keySize * 2, MAX_ALIGN) + map->valueSize * map->spare;
}

bool map_insert(Map *map, const void *key, const void *value) {
	Hash hash = map->hash_f(map, key);
	Hash hashbits = hash & HASH_MASK;
	u64 pos = hash % map->reserved;
	u64 dist = 0;
	bool isNew = true;

	while (true) {
		MapEntry *entry = map_entries(map) + pos;

		if (!entry->used) {
			*entry = (MapEntry){
				.dist = dist,
				.hash = hashbits,
				.used = true,
			};
			memcpy(map_keyAt(map, pos), key, map->keySize);
			memcpy(map_valueAt(map, pos), value, map->valueSize);
			break ;
		}
		else if (entry->dist == dist && entry->hash == hashbits && map->cmp_f(map, map_keyAt(map, pos), key)) {
			memcpy(map_keyAt(map, pos), key, map->keySize);
			memcpy(map_valueAt(map, pos), value, map->valueSize);
			break ;
		}
		else if (entry->dist < dist) {
			if (isNew) {
				map->used++;
				isNew = false;
			}

			MapEntry oEntry = *entry;
			
			entry->dist = dist; 
			entry->hash = hashbits;

			memcpy(map_spareKey(map), map_keyAt(map, pos), map->keySize); 
			memcpy(map_spareValue(map), map_valueAt(map, pos), map->valueSize); 
			memcpy(map_keyAt(map, pos), key, map->keySize);
			memcpy(map_valueAt(map, pos), value, map->valueSize);

			dist = oEntry.dist;
			hashbits = oEntry.hash;
			key = map_spareKey(map);
			value = map_spareValue(map);

			map->spare = 1 - map->spare; 
		}

		dist++;

		if (dist >= MAX_DISTANCE) {
			RehashReturn ret = map_rehash(map, REHASH_GROW | REHASH_FORCED); 
			if (ret != REHASH_HAPPENED) {
				if (ret != REHASH_ERROR)	
					error_msgSet("failed to insert element in map");
				return false;
			}
			map_insert(map, key, value);
			return true;
		}

		pos = (pos + 1) % map->reserved;
	}

	if (isNew)
		map->used++;

	if (map_rehash(map, REHASH_GROW) == REHASH_ERROR)
		return false;
	return true;
}

static u64 map_find(Map *map, const void *key) {
	Hash hash = map->hash_f(map, key);
	Hash hashbits = hash & HASH_MASK;
	u64 pos = hash % map->reserved;
	u64 dist = 0;

	MapEntry *entries = map_entries(map);
	while (true) {
		if (!entries[pos].used)
			return INVALID_POS;

		if (entries[pos].hash == hashbits && map->cmp_f(map, map_keyAt(map, pos), key))
			return pos;

		dist++;
		
		if (dist >= MAX_DISTANCE)
			return INVALID_POS;
		pos = (pos + 1) % map->reserved;
	}
}

void *map_get(Map *map, const void *key) {
	u64 pos = map_find(map, key);

	return pos == INVALID_POS ? NULL : map_valueAt(map, pos);
}

void map_destroy(Map *map) {
	if (!map)
		return ;
	allocator_free(map->allocator, map->data);
	allocator_free(map->allocator, map);
}

Hash map_hash_fixxedStr(Map *, const void *str) {
	return hash_add_str(0xdeadbeef, str);
}

bool map_cmp_fixxedStr(Map *map, const void *str1, const void *str2) {
	return !strncmp(str1, str2, map->keySize);
}
