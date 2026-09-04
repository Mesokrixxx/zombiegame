#ifndef UTIL_FILE_H
# define UTIL_FILE_H

# include "container/string.h"

typedef struct {
	Allocator *allocator;
	String path;
	String content;
} File;

File *file_create(Allocator *allocator, const char *path);
void file_setPath(File *file, const char *path);
bool file_load(File *file);
void file_destroy(File *file);

#endif // UTIL_FILE_H
