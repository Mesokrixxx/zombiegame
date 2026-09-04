#include "shader.h"
#include "util/stringview.h"
#include "util/error.h"
#include <string.h>
#include <GL/glew.h>

#define UNIFORMNAME_MAXLEN 32

GLShader *glshader_create(Allocator *allocator, const char *path) {
	GLShader *shader = allocator_alloc(allocator, sizeof(GLShader));
	File *shaderFile = file_create(allocator, path);
	Map *map = map_create(allocator, sizeof(char[UNIFORMNAME_MAXLEN]), sizeof(int), map_hash_fixxedStr, map_cmp_fixxedStr);
	String errorLogStr = string_create(allocator, NULL, STRING_ERRORLOG_MINSIZE);
	*shader = (GLShader){
		.allocator = allocator,
		.file = shaderFile,
		.uniforms = map,
		.errorLog = errorLogStr
	};

	return shader;
}

static u32 shader_compileShader(String errorLog, StringView zoneName, u32 gltype, StringView src) {
	u32 shader = glCreateShader(gltype);

	glShaderSource(shader, 1, &src.str, (int *)&src.len);
	glCompileShader(shader);

	int success;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		int minLength, written;
		u64 errorLogLen = string_length(errorLog);
		
		string_append(errorLog, "%s%.*s: ",
			(errorLogLen && errorLog[errorLogLen - 1] != '\n') ? "\n" : "", STRINGVIEW_FMT(zoneName));
		errorLogLen = string_length(errorLog);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &minLength);
		string_reserve(errorLog, errorLogLen + minLength);
		glGetShaderInfoLog(shader, minLength, &written, errorLog + errorLogLen);
		string_setLength(errorLog, errorLogLen + written);
		glDeleteShader(shader);
		return 0;
	}
	
	return shader;
}

static bool shader_compileFile(Dynlist(u32) *shaders, String fileContent, String errorLog) {
	typedef struct {
		StringView name;
		u32 glTypeTarget;
	} ShaderZone;

	const ShaderZone zones[] = {
		{ .name = STRINGVIEW("vertex"), .glTypeTarget = GL_VERTEX_SHADER },
		{ .name = STRINGVIEW("fragment"), .glTypeTarget = GL_FRAGMENT_SHADER },
	};

	bool ret = true;

	for (u64 i = 0; fileContent[i]; i++) {
		if (fileContent[i] != '@')
			continue ;
		
		StringView zoneNameLine = stringview_getLine(fileContent + i + 1);
		StringView zoneName = stringview_trim(zoneNameLine);
		u32 glType = 0;

		for (u64 j = 0; j < ARRAY_SIZE(zones); j++) {
			if (stringview_eq(zoneName, zones[j].name)) {
				glType = zones[j].glTypeTarget;
				break ;
			}
		}

		if (!glType) {
			u64 errorLogLen = string_length(errorLog);

			string_append(errorLog, "%s%.*s: unknown zone name", 
				(errorLogLen && errorLog[errorLogLen - 1] != '\n') ? "\n" : "", STRINGVIEW_FMT(zoneName));
			ret = false;
			continue ;
		}

		i += zoneNameLine.len + 1;
		StringView src = { 
			.str = fileContent + i
		};

		while (src.str[src.len] && src.str[src.len] != '@')
			src.len++;
		
		u32 compiledShader = shader_compileShader(errorLog, zoneName, glType, src);
		if (!compiledShader)
			ret = false;
		else
		 	dynlist_pushBack(*shaders, &compiledShader);
		i += src.len - 1;
	}

	return ret;
}

bool glshader_compile(GLShader *shader) {
	if (!file_load(shader->file))
		return false;

	Dynlist(u32) shaders;

	dynlist_init(shader->allocator, shaders, 4);
	if (!shader_compileFile(&shaders, shader->file->content, shader->errorLog)) {
		error_msgSet(shader->errorLog);
		dynlist_forEach(shaders, it)
			glDeleteShader(*it.elem);
		dynlist_destroy(shaders);
		return false;
	}

	u32 program = glCreateProgram();

	dynlist_forEach(shaders, it) {
		glAttachShader(program, *it.elem);
		glDeleteShader(*it.elem);
	}
	dynlist_destroy(shaders);

	int success;
	
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		int length, written;
		u64 errorLenLength = string_length(shader->errorLog);

		string_append(shader->errorLog, "%sprogram: ",
			(errorLenLength && shader->errorLog[errorLenLength - 1] != '\n' ? "\n" : ""));
		errorLenLength = string_length(shader->errorLog);
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		string_reserve(shader->errorLog, errorLenLength + length);
		glGetProgramInfoLog(program, length, &written, shader->errorLog + errorLenLength);
		string_setLength(shader->errorLog, errorLenLength + written);
		glDeleteProgram(program);
		error_msgSet(shader->errorLog);
		return false;
	}

	shader->handle = program;
	return true;
}

void glshader_bind(GLShader *shader) {
	static u32 boundShaderHandle;

	if (boundShaderHandle == shader->handle)
		return ;
	glUseProgram(shader->handle);
	boundShaderHandle = shader->handle;
}

void glshader_unbind() {
	GLShader shader = {0};

	glshader_bind(&shader);
}

bool glshader_setUniformMat4(GLShader *shader, const char *uniform, Mat4 *mat) {
	char buf[UNIFORMNAME_MAXLEN];

	strncpy(buf, uniform, UNIFORMNAME_MAXLEN);
	if (buf[UNIFORMNAME_MAXLEN - 1]) {
		string_assign(shader->errorLog, "uniform name is too long: max size: %zu", UNIFORMNAME_MAXLEN - 1);
		error_msgSet(shader->errorLog);
		return false;
	}

	int loc;
	int *mapVal = map_get(shader->uniforms, buf);
	if (!mapVal) {
		loc = glGetUniformLocation(shader->handle, buf);
		if (loc < 0) {
			error_msgSet("uniform name not found");
			return false;
		}
		map_insert(shader->uniforms, buf, &loc);
	}
	else 
		loc = *mapVal;

	glUniformMatrix4fv(loc, 1, GL_FALSE, mat->raw);
	return true;
}

void glshader_destroy(GLShader *shader) {
	if (!shader)
		return ;
	glDeleteProgram(shader->handle);
	map_destroy(shader->uniforms);
	file_destroy(shader->file);
	string_destroy(shader->errorLog);
	allocator_free(shader->allocator, shader);
}
