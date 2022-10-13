#pragma once
#include<GL/glew.h>
#include<GL/glut.h>
#include<glm/vec2.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat2x2.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/ext.hpp>
#include<glm/matrix.hpp>


#define VEC2 glm::vec2 
#define VEC3 glm::vec3
#define VEC4 glm::vec4
#define BUFFER_OFFSET( offset ) ((GLvoid*) (offset))
GLuint
InitShader(const char* vShaderFile, const char* fShaderFile);