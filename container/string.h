#ifndef CONTAINER_STRING_H
# define CONTAINER_STRING_H

# include "dynlist.h"
# include <stdarg.h>

# define STRING_ERRORLOG_MINSIZE 256

typedef Dynlist(char) String;

String string_create(Allocator *allocator, const char *cstr, u64 reserve);
void string_clear(String str);
void string_setLength(String str, u64 len);
void string_vassign(String str, const char *fmt, va_list ap);
void string_assign(String str, const char *fmt, ...);
void string_vappend(String str, const char *fmt, va_list ap);
void string_append(String str, const char *fmt, ...);

# define string_length dynlist_size
# define string_capacity dynlist_capacity
# define string_reserve dynlist_reserve
# define string_destroy dynlist_destroy

#endif // CONTAINER_STRING_H
