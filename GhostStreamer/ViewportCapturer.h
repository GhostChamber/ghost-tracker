#pragma once

#include "ViewportTexture.h"
#include "Constants.h"
#include "Types.h"
#include <Windows.h>
#include "Matrix.h"

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

	void SetWindow(LPCSTR windowStr, LPCSTR windowClass);

	void Update();

	void RenderIndividualQuadrant() const;

	void RenderAllQuadrants() const;

	void SetQuadrant(int32 quadrant);

	static void InitializeMeshArrays();
	
	static void FlipTexcoords();

	static void SetRenderingState();
	static void ClearRenderingState();

private:

	void Render(Matrix& rotationMatrix) const;

	void CopyPixelsFromScreen();

	void InitializeDC();
	void DestroyDC();

private:

	static float sPositionArray[2*5];
	static float sTexcoordArray[2*5];
	static const RECT sCaptureClip;

	ViewportTexture mViewportTexture;
	bool mIsActive;

	int32 mX;
	int32 mY;
	int32 mWidth;
	int32 mHeight;

	int32 mQuadrant;

	uint8* mPixelBuffer;

	HWND mSrcWindow;
	HDC mSrcWindowDC;

	HDC mCaptureDC;
	HBITMAP mCaptureBitmap;
};
