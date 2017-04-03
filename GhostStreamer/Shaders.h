#pragma once

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
"in vec2 aPosition;\n"
"in vec2 aTexcoord;\n"
"out vec2 vTexcoord;\n"
"uniform mat4 uMatrix;\n"
"uniform float uWidthScale;\n"
"uniform float uHeightScale;\n"

"void main()\n"
"{\n"
"    vTexcoord = vec2(aTexcoord.x * uWidthScale, aTexcoord.y * uHeightScale);\n"
"    gl_Position = uMatrix * vec4(aPosition, 0.0, 1.0);\n"
"}\n";

static const char* pColorMeshFragmentShader = 
GLSL_VERSION_STRING
"precision mediump float;\n"
"uniform sampler2D uTexture;\n"
"uniform int uTextureMode;\n"
"uniform vec4 uColor;\n"
"in vec2 vTexcoord;\n"
"out vec4 oFragColor;\n"

"void main()\n"
"{\n"
"    vec4 color;\n"
"    if (uTextureMode == 1)\n"
"    {\n"
"        color = texture(uTexture, vTexcoord);\n"
"    }"
"    else\n"
"    {\n"
"        color = uColor;\n"
"    }\n"
"    oFragColor = color;\n"
"}\n";
