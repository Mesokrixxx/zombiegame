#include "hash.h"

#define X(t) Hash hash_add_##t(Hash hash, t x) { return hash ^ (((Hash)(x)) + 0x9E3779B9u + (hash << 6) + (hash >> 2)); }
_HASH_ADD_LIST(X)
#undef X

Hash hash_add_str(Hash hash, const char *str) {
	while (*str) {
		hash = hash_add_char(hash, *str);
		str++;
	}
	return hash;
}
