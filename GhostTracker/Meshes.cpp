#include "OpenGL.h"
#include "Meshes.h"

#include "SphereMesh.h"
#include "ArrowMesh.h"
#include "CubeMesh.h"

static GLuint sMeshVBOHandles[NUM_MESHES] = {0};
static GLuint sMeshFaces[NUM_MESHES] = {0};

void LoadMeshes()
{
	glGenBuffers(NUM_MESHES, sMeshVBOHandles);

	// Sphere (0)
	glBindBuffer(GL_ARRAY_BUFFER, sMeshVBOHandles[SPHERE]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sSphereMeshVertexArray), sSphereMeshVertexArray, GL_STATIC_DRAW);
	sMeshFaces[0] = sSphereMeshFaceCount;

	// Arrow (1)
	glBindBuffer(GL_ARRAY_BUFFER, sMeshVBOHandles[ARROW]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sArrowMeshVertexArray), sArrowMeshVertexArray, GL_STATIC_DRAW);
	sMeshFaces[1] = sArrowMeshFaceCount;

}

void UnloadMeshes()
{
	glDeleteBuffers(NUM_MESHES, sMeshVBOHandles);
}

GLuint GetMeshVBOHandle(unsigned int index)
{
	GLuint handle = 0;

	if (index >= 0 &&
		index < NUM_MESHES)
	{
		handle = sMeshVBOHandles[index];
	}
	return handle;
}

GLuint GetMeshFaceCount(unsigned int index)
{
	GLuint handle = 0;

	if (index >= 0 &&
		index < Mesh::NUM_MESHES)
	{
		handle = sMeshFaces[index];
	}
	return handle;
}

