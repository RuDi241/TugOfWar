#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include "glad/glad.h"

GLuint compileShader(GLenum type, const char *source);
GLuint createShaderProgram(const char *vertexSrc, const char *fragmentSrc);

#endif
