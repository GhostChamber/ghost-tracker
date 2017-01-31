#pragma once

#include "Meshes.h"
#include "OpenGL.h"

enum ShaderPrograms
{
	SHADER_COLOR_MESH = 0,
	NUM_SHADERS
};

#define GLSL_VERSION_STRING "#version 300 es\n"

int LoadShaders();
void UnloadShaders();
GLuint GetShaderProgram(int nIndex);

static const char* pColorMeshVertexShader =
GLSL_VERSION_STRING
"in vec3 aPosition;\n"
"uniform mat4 uMatrixMVP;\n"

"void main()\n"
"{\n"
"    gl_Position = uMatrixMVP * vec4(aPosition, 1.0);\n"
//"    gl_Position = vec4(aPosition.x, aPosition.y, 0.0f, 1.0);\n"
"}\n";

static const char* pColorMeshFragmentShader = 
GLSL_VERSION_STRING
"precision mediump float;\n"
"uniform vec4 uColor;\n"
"out vec4 oFragColor;\n"

"void main()\n"
"{\n"
"    oFragColor = uColor;\n"
//"    oFragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\n";
