#pragma once

#include "ViewportTexture.h"
#include "Types.h"
#include "Constants.h"
#include <Windows.h>

class ViewportCapturer
{
public:

	ViewportCapturer();
	ViewportCapturer(int32 quadrant);
	~ViewportCapturer();

	void SetRect(int32 x,
				 int32 y,
				 int32 width,
				 int32 height);

	void Update();

	void RenderIndividualQuadrant() const;

	void RenderAllQuadrants() const;

	void SetQuadrant(int32 quadrant);

private:

	void CopyPixelsFromScreen();
	void CopyPixelsFromScreen_EXPERIMENTAL();

	void InitializeDC();
	void DestroyDC();

private:

	ViewportTexture mViewportTexture;

	int32 mX;
	int32 mY;
	int32 mWidth;
	int32 mHeight;

	int32 mQuadrant;

	uint8* mPixelBuffer;

	HDC mCaptureDC;
	HBITMAP mCaptureBitmap;
};