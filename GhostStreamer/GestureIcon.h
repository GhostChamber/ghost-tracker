#pragma once

#include "OpenGL.h"

class GestureIcon
{
public:

	GestureIcon();

	static void InitializeVertexArrays();
	static void LoadTextures();

	void Render();

	static void SetRenderingState();
	static void ClearRenderingState();

	void SetStreamManager(class StreamManager* manager);

private:

	class StreamManager* mStreamManager;

	static GLuint sTextureGrab;
	static GLuint sTextureZoom;
	static GLuint sTextureOrbit;

	static float sPositionArray[2 * 4];
	static float sTexcoordArray[2 * 4];

};