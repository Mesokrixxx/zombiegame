#ifndef MATH_MACROS_H
# define MATH_MACROS_H

# define MAX(a, b) ({ typeof(a + b) _a = (a), _b = (b); _a > _b ? _a : _b; })
# define MIN(a, b) ({ typeof(a + b) _a = (a), _b = (b); _a < _b ? _a : _b; })

#endif // MATH_MACROS_H
