#include "bitset.h"

const u64 ZERO = 0;

Bitset bitset_create(Allocator *allocator) {
	Bitset bset;

	dynlist_init(allocator, bset, 2);
	dynlist_pushBackN(bset, &ZERO, dynlist_capacity(bset));
	return bset;
}

void bitset_clear(Bitset bset) {
	dynlist_forEach(bset, it) {
		*it.elem = 0; 
	}
}

void bitset_set(Bitset bset, u64 idx, bool enabled) {
	u64 listIdx = idx / 64;
	u64 off = idx % 64;

	i64 toAdd = listIdx + 1 - dynlist_capacity(bset);
	if (toAdd > 0)
		dynlist_pushBackN(bset, &ZERO, toAdd);

	u64 toEnable = (u64)1 << off; 	
	if (enabled)
		bset[listIdx] |= toEnable;
	else
	 	bset[listIdx] &= ~toEnable;
}

bool bitset_eq(Bitset bset1, Bitset bset2) {
	Bitset big, small;

	if (dynlist_size(bset1) > dynlist_size(bset2)) {
		big = bset1;
		small = bset2;
	}
	else {
		big = bset2;
		small = bset1;
	}

	dynlist_forEach(small, it) {
		if (*it.elem != big[it.idx])
			return false;
	}

	dynlist_forEachOffset(big, dynlist_size(small), it) {
		if (*it.elem)
			return false;
	}

	return true;
}

bool bitset_bit(Bitset bset, u64 idx) {
	u64 listIdx = idx / 64;
	u64 toFind = 1 << (idx % 64);

	return dynlist_size(bset) > listIdx	? bset[listIdx] & toFind  : false;
}

bool bitset_has(Bitset bset, Bitset has) {
	u64 bsetSize = dynlist_size(bset);

	if (bsetSize > dynlist_size(has)) {
		dynlist_forEach(has, it) {
			if ((bset[it.idx] & *it.elem) != *it.elem)
				return false;
		}
	}

	else {
		dynlist_forEach(bset, it) {
			if ((*it.elem & has[it.idx]) != has[it.idx])
				return false;
		}

		dynlist_forEachOffset(has, bsetSize, it) {
			if (*it.elem)
				return false;
		}
	}

	return true;
}
