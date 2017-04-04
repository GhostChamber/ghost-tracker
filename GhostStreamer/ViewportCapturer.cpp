#include "ViewportCapturer.h"
#include "ViewportTexture.h"
#include <string.h>
#include <stdio.h>
#include <Windows.h>
#include "Matrix.h"
#include "OpenGL.h"
#include "Shaders.h"

float ViewportCapturer::sPositionArray[2 * 5] = {0};
float ViewportCapturer::sTexcoordArray[2 * 5] = {0};
const RECT ViewportCapturer::sCaptureClip = { 0, 85, 120, 90 };

ViewportCapturer::ViewportCapturer() :
	mIsActive(false),
	mX(0),
	mY(0),
	mWidth(0),
	mHeight(0),
	mQuadrant(0),
	mSrcWindow(nullptr),
	mSrcWindowDC(nullptr),
	mCaptureDC(nullptr),
	mCaptureBitmap(nullptr)
{
	mPixelBuffer = new uint8[MAX_VIEWPORT_WIDTH * MAX_VIEWPORT_HEIGHT * 4];
	memset(mPixelBuffer, 0, MAX_VIEWPORT_WIDTH * MAX_VIEWPORT_HEIGHT * 4);

	InitializeDC();
	mViewportTexture.Initialize();
}

ViewportCapturer::ViewportCapturer(int32 quadrant) :
	ViewportCapturer()
{
	mQuadrant = quadrant;
}

ViewportCapturer::~ViewportCapturer()
{
	DestroyDC();

	delete[] mPixelBuffer;
	mPixelBuffer = nullptr;
}

void ViewportCapturer::SetRect(int32 x, int32 y, int32 width, int32 height)
{
	mSrcWindow = GetDesktopWindow();
	mSrcWindowDC = GetDC(mSrcWindow);
	mX = x;
	mY = y;
	mWidth = width;
	mHeight = height;

	if (width > MAX_VIEWPORT_WIDTH)
	{
		printf("Max viewport width surpassed for viewport %d\n", mQuadrant);
		return;
	}
	if (height > MAX_VIEWPORT_HEIGHT)
	{
		printf("Max viewport height surpassed for viewport %d\n", mQuadrant);
		return;
	}
	mIsActive = true;
}

void ViewportCapturer::SetWindow(LPCSTR windowName, LPCSTR windowClass)
{
	mSrcWindow = FindWindow(windowClass, windowName);
	if (mSrcWindow != nullptr)
	{
		mSrcWindowDC = GetDC(mSrcWindow);
		RECT windowRect;
		GetWindowRect(mSrcWindow, &windowRect);
		mWidth = (windowRect.right - windowRect.left) - (sCaptureClip.left + sCaptureClip.right);
		mHeight = (windowRect.bottom - windowRect.top) - (sCaptureClip.top + sCaptureClip.bottom);
		mX = windowRect.left + sCaptureClip.left;
		mY = windowRect.top + sCaptureClip.top;
		mIsActive = true;
	}
}

void ViewportCapturer::Update()
{
	if (mIsActive)
	{
		CopyPixelsFromScreen();

		mViewportTexture.StreamPixelsToGPU(mPixelBuffer, mWidth, mHeight);
	}
}

void ViewportCapturer::RenderIndividualQuadrant() const
{
	if (mIsActive)
	{
		// Assumed that caller has enabled rendering state with 
		// ViewportCapturer::SetRenderingState().

		Matrix rotationMatrix;
		rotationMatrix.Rotate(90.0f * mQuadrant, 0.0f, 0.0f, 1.0f);
		Render(rotationMatrix);
	}
}

void ViewportCapturer::RenderAllQuadrants() const
{
	if (mWidth != 0 &&
		mHeight != 0 &&
		mViewportTexture.GetTextureID() != 0U)
	{
		// Assumed that caller has enabled rendering state with 
		// ViewportCapturer::SetRenderingState().
		Matrix rotationMatrix;
		for (int i = 0; i < NUM_DISPLAY_QUADRANTS; i++)
		{
			rotationMatrix.Rotate(90.0f, 0.0f, 0.0f, 1.0f);
			Render(rotationMatrix);
		}
	}
}

void ViewportCapturer::Render(Matrix& rotationMatrix) const
{
	GLuint hProg = GetShaderProgram(SHADER_COLOR_MESH);

	GLint hPosition = glGetAttribLocation(hProg, "aPosition");
	GLint hTexcoord = glGetAttribLocation(hProg, "aTexcoord");

	glEnableVertexAttribArray(hPosition);
	glEnableVertexAttribArray(hTexcoord);

	glVertexAttribPointer(hPosition,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		sPositionArray);
	glVertexAttribPointer(hTexcoord,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		sTexcoordArray);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mViewportTexture.GetTextureID());

	GLint hTexture = glGetUniformLocation(hProg, "uTexture");
	GLint hMatrix = glGetUniformLocation(hProg, "uMatrix");
	GLint hWidthScale = glGetUniformLocation(hProg, "uWidthScale");
	GLint hHeightScale = glGetUniformLocation(hProg, "uHeightScale");
	GLint hTextureMode = glGetUniformLocation(hProg, "uTextureMode");
	GLint hColor = glGetUniformLocation(hProg, "uColor");

	glUniform1i(hTexture, 0);
	glUniformMatrix4fv(hMatrix, 1, GL_FALSE, rotationMatrix.GetArray());
	glUniform1f(hWidthScale, static_cast<float>(mWidth) / MAX_VIEWPORT_WIDTH);
	glUniform1f(hHeightScale, static_cast<float>(mHeight) / MAX_VIEWPORT_HEIGHT);
	glUniform1i(hTextureMode, 1);

	float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	glUniform4fv(hColor, 1, color);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
}

void ViewportCapturer::SetQuadrant(int32 quadrant)
{
	mQuadrant = quadrant;
}

void ViewportCapturer::InitializeMeshArrays()
{
	// Top left
	sPositionArray[0] = -1.0f;
	sPositionArray[1] = 1.0f;
	sTexcoordArray[0] = 0.0f;
	sTexcoordArray[1] = 1.0f;

	// Bottom left
	sPositionArray[2] = 0.0f - CENTER_OFFSET;
	sPositionArray[3] = 0.0f + CENTER_OFFSET;
	sTexcoordArray[2] = 0.5f - CENTER_OFFSET / 2;
	sTexcoordArray[3] = 0.0f;

	// Top middle
	sPositionArray[4] = 0.0f;
	sPositionArray[5] = 1.0f;
	sTexcoordArray[4] = 0.5f;
	sTexcoordArray[5] = 1.0f;

	// Bottom right
	sPositionArray[6] = 0.0f + CENTER_OFFSET;
	sPositionArray[7] = 0.0f + CENTER_OFFSET;
	sTexcoordArray[6] = 0.5f + CENTER_OFFSET / 2;
	sTexcoordArray[7] = 0.0f;

	// Top right
	sPositionArray[8] = 1.0f;
	sPositionArray[9] = 1.0f;
	sTexcoordArray[8] = 1.0f;
	sTexcoordArray[9] = 1.0f;
}

void ViewportCapturer::FlipTexcoords()
{
	sTexcoordArray[1] = 1.0f - sTexcoordArray[1];
	sTexcoordArray[3] = 1.0f - sTexcoordArray[3];
	sTexcoordArray[5] = 1.0f - sTexcoordArray[5];
	sTexcoordArray[7] = 1.0f - sTexcoordArray[7];
	sTexcoordArray[9] = 1.0f - sTexcoordArray[9];
}

void ViewportCapturer::SetRenderingState()
{
	glUseProgram(GetShaderProgram(SHADER_COLOR_MESH));
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
}

void ViewportCapturer::ClearRenderingState()
{
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
}

void ViewportCapturer::CopyPixelsFromScreen()
{
	SelectObject(mCaptureDC, mCaptureBitmap);
	BitBlt(mCaptureDC, 0, 0, mWidth, mHeight, mSrcWindowDC, mX, mY, SRCCOPY | CAPTUREBLT);

	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = mWidth;
	bmi.bmiHeader.biHeight = mHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	GetDIBits(mCaptureDC,
		mCaptureBitmap,
		0,
		mHeight,
		mPixelBuffer,
		&bmi,
		DIB_RGB_COLORS);

	ReleaseDC(mSrcWindow, mSrcWindowDC);
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
