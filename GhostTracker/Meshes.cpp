#include "OpenGL.h"
#include "Meshes.h"
#include "SphereMesh.h"
#include "CubeMesh.h"

#include "SphereMesh.h"
#include "CubeMesh.h"

static GLuint sMeshVBOHandles[NUM_MESHES] = {0};

void LoadMeshes()
{
	for (int i = 0; i < NUM_MESHES; i++)
	{
		;
	}
}

void UnloadMeshes()
{

}

GLuint GetMeshVBOHandle(int index)
{
	GLuint handle = 0;

	if (index >= 0 &&
		index < NUM_MESHES)
	{
		handle = sMeshVBOHandles[index];
	}
	return handle;
}
