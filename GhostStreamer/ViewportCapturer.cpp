#include "ViewportCapturer.h"
#include "ViewportTexture.h"
#include <string.h>
#include <stdio.h>
#include <Windows.h>

ViewportCapturer::ViewportCapturer() :
	mX(0),
	mY(0),
	mWidth(0),
	mHeight(0),
	mQuadrant(0),
	mCaptureDC(0),
	mCaptureBitmap(0)
{
	mPixelBuffer = new uint8[MAX_VIEWPORT_WIDTH * MAX_VIEWPORT_HEIGHT * 4];
	memset(mPixelBuffer, 0, MAX_VIEWPORT_WIDTH * MAX_VIEWPORT_HEIGHT * 4);

	//InitializeDC();
}

ViewportCapturer::ViewportCapturer(int32 quadrant) :
	ViewportCapturer()
{
	mQuadrant = quadrant;
}

ViewportCapturer::~ViewportCapturer()
{
	//DestroyDC();

	delete [] mPixelBuffer;
	mPixelBuffer = nullptr;
}

void ViewportCapturer::SetRect(int32 x,
							   int32 y,
							   int32 width,
							   int32 height)
{
	mX = x;
	mY = y;
	mWidth = width;
	mHeight = height;

	if (width > MAX_VIEWPORT_WIDTH)
	{
		printf("Max viewport width surpassed for viewport %d\n", mQuadrant);
	}
	if (height > MAX_VIEWPORT_HEIGHT)
	{
		printf("Max viewport height surpassed for viewport %d\n", mQuadrant);
	}
}

void ViewportCapturer::Update()
{
	if (mWidth > 0 &&
		mWidth < MAX_VIEWPORT_WIDTH &&
		mHeight > 0 &&
		mHeight < MAX_VIEWPORT_HEIGHT &&
		mViewportTexture.GetTextureID() != 0)
	{
		//CopyPixelsFromScreen();
		CopyPixelsFromScreen_EXPERIMENTAL();

		mViewportTexture.StreamPixelsToGPU(mPixelBuffer);
	}
}

void ViewportCapturer::RenderIndividualQuadrant() const
{
	// TODO: Render logic for quadrant of screen
}

void ViewportCapturer::RenderAllQuadrants() const 
{
	// TODO: Render logic for rendering to entire screen 4 times.
	for (int i = 0; i < NUM_DISPLAY_QUADRANTS; i++)
	{
		
	}
}

void ViewportCapturer::SetQuadrant(int32 quadrant)
{
	mQuadrant = quadrant;
}

void ViewportCapturer::CopyPixelsFromScreen_EXPERIMENTAL()
{
	HWND hDesktopWnd = GetDesktopWindow();
	HDC hDesktopDC = GetDC(hDesktopWnd);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, mWidth, mHeight);
	SelectObject(hCaptureDC, hCaptureBitmap);

	BitBlt(hCaptureDC, 0, 0, mWidth, mHeight, hDesktopDC, mX, mY, SRCCOPY | CAPTUREBLT);

	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = mWidth;
	bmi.bmiHeader.biHeight = mHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	GetDIBits(hCaptureDC,
			  hCaptureBitmap,
			  0,
			  mHeight,
			  mPixelBuffer,
			  &bmi,
			  DIB_RGB_COLORS);
}

void ViewportCapturer::CopyPixelsFromScreen()
{
	HWND hDesktopWnd = GetDesktopWindow();
	HDC hDesktopDC = GetDC(hDesktopWnd);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
}

void ViewportCapturer::InitializeDC()
{
	HWND hDesktopWnd = GetDesktopWindow();
	HDC hDesktopDC = GetDC(hDesktopWnd);
	mCaptureDC = CreateCompatibleDC(hDesktopDC);
	mCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, MAX_VIEWPORT_WIDTH, MAX_VIEWPORT_HEIGHT);

	ReleaseDC(hDesktopWnd, hDesktopDC);

}

void ViewportCapturer::DestroyDC()
{
	DeleteDC(mCaptureDC);
	DeleteObject(mCaptureBitmap);

	mCaptureDC = 0;
	mCaptureBitmap = 0;
}
