#include "ViewportTexture.h"
#include "Constants.h"
#include "OpenGL.h"

ViewportTexture::ViewportTexture() :
	mTextureID(0),
	mAllocatedWidth(0),
	mAllocatedHeight(0),
	mWidth(0),
	mHeight(0)
{

}

ViewportTexture::~ViewportTexture()
{

}

void ViewportTexture::Initialize()
{
	if (mTextureID == 0U)
	{
		glGenTextures(1, &mTextureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTextureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 MAX_VIEWPORT_WIDTH,
					 MAX_VIEWPORT_HEIGHT,
					 0,
					 GL_BGRA,
					 GL_UNSIGNED_BYTE,
					 0);

		mAllocatedWidth = MAX_VIEWPORT_WIDTH;
		mAllocatedHeight = MAX_VIEWPORT_HEIGHT;
	}
}

void ViewportTexture::Destroy()
{
	if (mTextureID != 0U)
	{
		glDeleteTextures(1, &mTextureID);
		mTextureID = 0U;
	}
}

void ViewportTexture::StreamPixelsToGPU(uint8* pixels, 
										int32 width, 
										int32 height)
{
	if (mTextureID != 0U)
	{
		glBindTexture(GL_TEXTURE_2D, mTextureID);

		glTexSubImage2D(GL_TEXTURE_2D,
			0,
			0,
			0,
			width,
			height,
			GL_BGRA,
			GL_UNSIGNED_BYTE,
			pixels);

		mWidth = width;
		mHeight = height;
	}
}

GLuint ViewportTexture::GetTextureID() const 
{
	return mTextureID;
}
