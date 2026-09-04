#ifndef ENGINE_TIME_H
# define ENGINE_TIME_H

# include "util/types.h"

typedef u64 Time;

// return elasped time in ns since first time_now()
Time time_now();

#endif // ENGINE_TIME_H
