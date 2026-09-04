#ifndef UTIL_ERROR_H
# define UTIL_ERROR_H

# include "util/types.h"

const char *error_msgGet();
void error_msgSet(const char *msg);
void error_msgSetWithErrno();

#endif // UTIL_ERROR_H
