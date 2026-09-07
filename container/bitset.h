#ifndef CONTAINER_BITSET_H
# define CONTAINER_BITSET_H

# include "container/dynlist.h"

typedef Dynlist(u64) Bitset;

Bitset bitset_create(Allocator *allocator);
void bitset_clear(Bitset bset);
void bitset_set(Bitset bset, u64 idx, bool enabled);
bool bitset_eq(Bitset bset1, Bitset bset2);
bool bitset_bit(Bitset bset, u64 idx);
bool bitset_has(Bitset bset, Bitset has);

# define bitset_copy dynlist_copy
# define bitset_destroy dynlist_destroy

#endif // CONTAINER_BITSET_H
