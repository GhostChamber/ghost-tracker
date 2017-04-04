#include "Shaders.h"
#include "OpenGL.h"
#include <stdio.h>
#include <string.h>

#define LOG_SIZE 2048

static GLuint s_arPrograms[NUM_SHADERS] = {0};

void UnloadShaders()
{
	for (int i = 0; i < NUM_SHADERS; i++)
	{
		glDeleteShader(s_arPrograms[i]);
		s_arPrograms[i] = 0;
	}
}

static int BuildProgram(int          nProgramIndex,
						const char*  pVertex,
						const char*  pFragment)
{
	char arLogBuffer[LOG_SIZE] = {0};
	int nStatus   = 1;
	int nCompiled = 0;
	int nLinked   = 0;

	unsigned int hVertex   = glCreateShader(GL_VERTEX_SHADER);
	unsigned int hFragment = glCreateShader(GL_FRAGMENT_SHADER);
	unsigned int hProgram  = glCreateProgram();

	// Assign shader source code to strings passed into function.
	glShaderSource(hVertex, // Handle to Shader
				   1, // Number of strings in source array
				   &pVertex, // Pointer to address of first (and only) string
				   0); // Length array, 0 = strings are null terminated
	glShaderSource(hFragment,
				   1,
				   &pFragment,
				   0);

	// Compile individual shaders
	glCompileShader(hVertex);
	glCompileShader(hFragment);

	// Check compilation status of vertex shader
	glGetShaderiv(hVertex,
				  GL_COMPILE_STATUS,
				  &nCompiled);

	// If shader didnt compile, print info log.
	if (nCompiled == 0)
	{
		// Clear buffer to hold 
		memset(arLogBuffer, 0, LOG_SIZE);

		// Get compilation log
		glGetShaderInfoLog(hVertex,        // Handle to shader
						   LOG_SIZE - 1,   // Max size of buffer
						   0,              // Returned actual size, 0 = don't care
						   arLogBuffer);    // Pointer to buffer

		printf("Vertex shader failed to compile");
		printf(arLogBuffer);

		nStatus = 0;
	}

	// Check compilation status of fragment shader
	glGetShaderiv(hFragment,
				  GL_COMPILE_STATUS,
				  &nCompiled);

	// If shader didnt compile, print info log.
	if (nCompiled == 0)
	{
		// Clear buffer to hold 
		memset(arLogBuffer, 0, LOG_SIZE);

		// Get compilation log
		glGetShaderInfoLog(hFragment,      // Handle to shader
						   LOG_SIZE - 1,   // Max size of buffer
						   0,              // Returned actual size, 0 = don't care
						   arLogBuffer);    // Pointer to buffer

		printf("Fragment shader failed to compile");
		printf(arLogBuffer);

		nStatus = 0;
	}

	// If both shaders compiled succesfully, attach them to the program
	// object and link them.
	if (nStatus != 0)
	{
		// Attach individual shaders to the shader program
		glAttachShader(hProgram, hVertex);
		glAttachShader(hProgram, hFragment);
		glBindFragDataLocation(hProgram, 0, "oFragColor");

		// Link program
		glLinkProgram(hProgram);

		// Check if program linked successfully.
		glGetProgramiv(hProgram,
					   GL_LINK_STATUS,
					   &nLinked);

		// If program didn't link correctly, log an error 
		// and return failure.
		if (nLinked == 0)
		{
			printf("Shader program failed to link");
			nStatus = 0;
		}
		// Otherwise assign the shader program to specified target
		// in the program array.
		else
		{
			s_arPrograms[nProgramIndex] = hProgram;
		}
	}
	return nStatus;
}

//*****************************************************************************
// LoadShaders
//*****************************************************************************
int LoadShaders()
{
	int nStatus = 1;

	// Clear all the shader program values to 0.
	memset(s_arPrograms, 0, NUM_SHADERS * sizeof(int));

	// Quad Shader
	if (nStatus != 0)
	{
		printf("Building Color Mesh Shader");
		nStatus = BuildProgram(SHADER_COLOR_MESH,
							   pColorMeshVertexShader,
							   pColorMeshFragmentShader);
	}

	if (nStatus != 0)
	{
		printf("Building Color Mesh Shader");
		nStatus = BuildProgram(SHADER_ICON,
			pIconVertexShader,
			pIconFragmentShader);
	}

	return nStatus;
}

//*****************************************************************************
// GetShaderProgram
//*****************************************************************************
GLuint GetShaderProgram(int nIndex)
{
	unsigned int hProg = 0;

	if (nIndex >= 0 &&
		nIndex < NUM_SHADERS)
	{
		hProg = s_arPrograms[nIndex];
	}

	return hProg;
}
