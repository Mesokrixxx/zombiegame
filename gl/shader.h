#ifndef GL_SHADER_H
# define GL_SHADER_H

# include "container/map.h"
# include "util/file.h"
# include "math/mat4.h"

typedef struct {
	Allocator *allocator;
	File *file;
	u32 handle;
	Map *uniforms;
	String errorLog;
} GLShader;

GLShader *glshader_create(Allocator *allocator, const char *path);
bool glshader_compile(GLShader *shader);
void glshader_bind(GLShader *shader);
void glshader_unbind();
bool glshader_setUniformMat4(GLShader *shader, const char *uniform, Mat4 *mat);
void glshader_destroy(GLShader *shader);

#endif // GL_SHADER_H
