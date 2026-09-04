#include "error.h"
#include <string.h>
#include <errno.h>

const char *errorMsg = 0;

const char *error_msgGet() {
	return errorMsg;
}

void error_msgSet(const char *msg) {
	errorMsg = msg;
}

void error_msgSetWithErrno() {
	errorMsg = strerror(errno);
}
