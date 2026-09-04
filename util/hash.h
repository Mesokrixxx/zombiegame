#ifndef UTIL_HASH_H
# define UTIL_HASH_H

# include "util/types.h"

typedef u64 Hash;

# define _HASH_ADD_LIST(X) \
	X(char)

# define X(t) Hash hash_add_##t(Hash hash, t x);
_HASH_ADD_LIST(X)
# undef X

Hash hash_add_str(Hash hash, const char *str);

#endif // UTIL_HASH_H
