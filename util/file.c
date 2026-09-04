#include "file.h"
#include "util/error.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

File *file_create(Allocator *allocator, const char *path) {
	File *file = allocator_alloc(allocator, sizeof(File));
	String pathStr = string_create(allocator, path, path ? 0 : 32);
	*file = (File){
		.allocator = allocator,
		.path = pathStr
	};

	return file;
}

void file_setPath(File *file, const char *path) {
	string_assign(file->path, "%s", path);
}

bool file_load(File *file) {
	int fd = open(file->path, O_RDONLY);
	if (fd < 0) {
		error_msgSetWithErrno();
		return false;
	}

	struct stat st;
	if (fstat(fd, &st) < 0) {
		error_msgSetWithErrno();
		close(fd);
		return false;
	}

	u64 fileSize = st.st_size;
	if (file->content) {
		string_clear(file->content);
		string_reserve(file->content, fileSize + 1);
	}
	else {
		file->content = string_create(file->allocator, NULL, fileSize + 1);
	}
	string_setLength(file->content, fileSize);

	u64 iFileSize = fileSize;
	while (fileSize) {
		i64 readRet = read(fd, file->content + iFileSize - fileSize, fileSize);
		
		if (readRet < 0) {
			error_msgSetWithErrno();
			close(fd);
			return false;
		}
		fileSize -= readRet;
	}
	file->content[iFileSize] = 0;

	close(fd);
	return true;
}

void file_destroy(File *file) {
	if (!file)
		return ;
	string_destroy(file->content);
	string_destroy(file->path);
	allocator_free(file->allocator, file);
}
