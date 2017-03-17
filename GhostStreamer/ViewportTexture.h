#pragma once

#include "Types.h"
#include "OpenGL.h"

class ViewportTexture
{
public:

	ViewportTexture();
	~ViewportTexture();

	void Initialize();
	void Destroy();

	void StreamPixelsToGPU(uint8* pixels);

	GLuint GetTextureID() const;

private:

	GLuint mTextureID;
	GLuint mAllocatedWidth;
	GLuint mAllocatedHeight;
	GLuint mWidth;
	GLuint mHeight;
};