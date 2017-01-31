#pragma once

#include "OpenGL.h"

enum Mesh
{
	SPHERE,
	NUM_MESHES
};

void LoadMeshes();

void UnloadMeshes();

GLuint GetMeshVBOHandle(unsigned int index);

GLuint GetMeshFaceCount(unsigned int index);
