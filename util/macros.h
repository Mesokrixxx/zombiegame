#ifndef UTIL_MACROS_H
# define UTIL_MACROS_H

# define ARRAY_SIZE(array) (sizeof(array) / sizeof(*(array)))

# define _CONCAT_IMPL(a, b) a ## b
# define CONCAT(a, b) _CONCAT_IMPL(a, b)

#endif // UTIL_MACROS_H
