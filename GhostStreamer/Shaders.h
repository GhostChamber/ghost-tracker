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
//"    gl_Position = vec4(aPosition, 0.0f, 1.0);\n"
"}\n";

static const char* pColorMeshFragmentShader = 
GLSL_VERSION_STRING
"precision mediump float;\n"
"uniform sampler2D uTexture;\n"
"in vec2 vTexcoord;\n"
"out vec4 oFragColor;\n"

"void main()\n"
"{\n"
"    vec4 color = texture2D(uTexture, vTexcoord);\n"
"    float value = 0.0;\n"
"    value = color.r + color.g + color.b;\n"
"    color.rgb = color.rgb * float(value > 0.4);\n"
"    oFragColor = color;\n"
//"   oFragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n";
