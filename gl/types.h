#ifndef GL_TYPES_H
# define GL_TYPES_H

# include "util/types.h"

typedef enum : u8 {
	GLTYPES_F32,
	GLTYPES_U8,
} GLType;

typedef struct {
	u32 glTarget;
	u64 size;
} GLTypeInfo;

GLTypeInfo gltypes_getInfo(GLType type);

#endif // GL_TYPES_H
