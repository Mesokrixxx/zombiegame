#ifndef MEMORY_MALLOCATOR_H
# define MEMORY_MALLOCATOR_H

# include "memory/allocator.h"

typedef Allocator Mallocator;

void mallocator_init(Mallocator *mallocator);

#endif // MEMORY_MALLOCATOR_H
