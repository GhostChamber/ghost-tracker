#pragma once

enum Meshes
{
	MESH_SPHERE,
	MESH_CUBE,
	NUM_MESHES
};

void LoadMeshes();

void UnloadMeshes();

GLuint GetMeshVBOHandle();
