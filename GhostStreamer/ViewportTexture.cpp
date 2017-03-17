#include "ViewportTexture.h"

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

}

void ViewportTexture::Destroy()
{

}

void ViewportTexture::StreamPixelsToGPU(uint8* pixels)
{

}

GLuint ViewportTexture::GetTextureID() const 
{
	return mTextureID;
}
