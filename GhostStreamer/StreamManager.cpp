#include "StreamManager.h"
#include "Constants.h"
#include "Enums.h"
#include "ViewportCapturer.h"
#include <Windows.h>
#include <assert.h>
#include <stdio.h>
#include <SDL_syswm.h>
#include "Shaders.h"

float StreamManager::sUIPositionArray[2 * 4] = { 0 };
float StreamManager::sUITexcoordArray[2 * 4] = { 0 };

StreamManager::StreamManager() :
	mStreamType(StreamType::REPLICATED),
	mCommandState(CommandState::LISTENING),
	mPoint1X(0),
	mPoint1Y(0),
	mPoint2X(0),
	mPoint2Y(0),
	mCurrentGesture(GhostGesture::GRAB)
{
	for (int32 i = 0; i < NUM_DISPLAY_QUADRANTS; i++)
	{
		mIndividualCapturers[i].SetQuadrant(i);
	}

	ViewportCapturer::InitializeMeshArrays();
	InitializeUIVertexArrays();
}

StreamManager::~StreamManager()
{

}

void StreamManager::Update()
{
	mInputState.Update();

	for (int i = 0; i < NUM_DISPLAY_QUADRANTS; i++)
	{
		mIndividualCapturers[i].Update();
	}

	mReplicatedCapturer.Update();
}

void StreamManager::Render()
{
	// Render Viewports
	ViewportCapturer::SetRenderingState();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (mStreamType == StreamType::REPLICATED)
	{
		mReplicatedCapturer.RenderAllQuadrants();
	}
	else
	{
		for (int i = 0; i < NUM_DISPLAY_QUADRANTS; i++)
		{
			mIndividualCapturers[i].RenderIndividualQuadrant();
		}
	}

	RenderUiIcon();

	ViewportCapturer::ClearRenderingState();
}

void StreamManager::SetCurrentGesture(GhostGesture gesture)
{
	mCurrentGesture = gesture;
}

void StreamManager::RenderUiIcon()
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
		sUIPositionArray);
	glVertexAttribPointer(hTexcoord,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		sUITexcoordArray);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, 0);

	GLint hTexture = glGetUniformLocation(hProg, "uTexture");
	GLint hMatrix = glGetUniformLocation(hProg, "uMatrix");
	GLint hWidthScale = glGetUniformLocation(hProg, "uWidthScale");
	GLint hHeightScale = glGetUniformLocation(hProg, "uHeightScale");
	GLint hTextureMode = glGetUniformLocation(hProg, "uTextureMode");
	GLint hColor = glGetUniformLocation(hProg, "uColor");

	glUniform1i(hTexture, 0);
	glUniform1f(hWidthScale, 1.0f);
	glUniform1f(hHeightScale, 1.0f);
	glUniform1i(hTextureMode, 0);
	
	float color[4] = { 0.0f };

	switch (mCurrentGesture)
	{
	case GhostGesture::NONE:
		color[0] = 0.0f;
		color[1] = 0.0f;
		color[2] = 0.0f;
		color[3] = 0.0f;
		break;
	case GhostGesture::GRAB :
		color[0] = 1.0f;
		color[1] = 0.0f;
		color[2] = 0.0f;
		color[3] = 0.4f;
		break;
	case GhostGesture::ZOOM :
		color[0] = 0.0f;
		color[1] = 1.0f;
		color[2] = 0.0f;
		color[3] = 0.4f;
		break;
	case GhostGesture::ORBIT :
		color[0] = 0.0f;
		color[1] = 0.0f;
		color[2] = 1.0f;
		color[3] = 0.4f;
		break;
	}

	glUniform4fv(hColor, 1, color);

	Matrix matrix;
	for (int i = 0; i < 4; i++)
	{
		matrix.Rotate(90.0f, 0.0f, 0.0f, 1.0f);
		glUniformMatrix4fv(hMatrix, 1, GL_FALSE, matrix.GetArray());

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}


void StreamManager::ActivateHotkey(WPARAM wparam, LPARAM lparam)
{
	if (mCommandState == CommandState::LISTENING)
	{
		switch (HIWORD(lparam))
		{
		case '1':
			mCommandState = CommandState::WAITING_FIRST_POINT;
			mCommandedCapturer = &mIndividualCapturers[0];
			break;
		case '2':
			mCommandState = CommandState::WAITING_FIRST_POINT;
			mCommandedCapturer = &mIndividualCapturers[1];
			break;
		case '3':
			mCommandState = CommandState::WAITING_FIRST_POINT;
			mCommandedCapturer = &mIndividualCapturers[2];
			break;
		case '4':
			mCommandState = CommandState::WAITING_FIRST_POINT;
			mCommandedCapturer = &mIndividualCapturers[3];
			break;

		case '6':
			ViewportCapturer::FlipTexcoords();
			break;

		case '7':
			// Toggle Stream Type
			if (mStreamType == StreamType::INDIVIDUAL)
			{
				mStreamType = StreamType::REPLICATED;
			}
			else
			{
				mStreamType = StreamType::INDIVIDUAL;
			}
			break;

		case '0':
			mCommandState = CommandState::WAITING_FIRST_POINT;
			mCommandedCapturer = &mReplicatedCapturer;
			break;
		}
	}
	
	else if (HIWORD(lparam) == 'X')
	{
		POINT p;
		GetCursorPos(&p);

		// Debug string
		char positionString[32] = {0};
		sprintf_s<32>(positionString, "%d, %d", p.x, p.y);

		if (mCommandState == CommandState::WAITING_FIRST_POINT)
		{
			POINT p;
			GetCursorPos(&p);
			mPoint1X = p.x;
			mPoint1Y = p.y;

			mCommandState = CommandState::WAITING_SECOND_POINT;
		}
		else if (mCommandState == CommandState::WAITING_SECOND_POINT)
		{
			POINT p;
			GetCursorPos(&p);
			mPoint2X = p.x;
			mPoint2Y = p.y;

			assert(mCommandedCapturer != nullptr);

			mCommandedCapturer->SetRect(mPoint1X,
				mPoint1Y,
				mPoint2X - mPoint1X,
				mPoint2Y - mPoint1Y);

			mCommandedCapturer = nullptr;
			mCommandState = CommandState::LISTENING;
		}
	}
}

void StreamManager::SetupHotkeys(SDL_Window* window) const
{
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);

	RegisterHotKey(wmInfo.info.win.window, static_cast<int32>(HotkeyID::COMMAND_VIEWPORT_1), MOD_CONTROL | MOD_ALT, '1');
	RegisterHotKey(wmInfo.info.win.window, static_cast<int32>(HotkeyID::COMMAND_VIEWPORT_2), MOD_CONTROL | MOD_ALT, '2');
	RegisterHotKey(wmInfo.info.win.window, static_cast<int32>(HotkeyID::COMMAND_VIEWPORT_3), MOD_CONTROL | MOD_ALT, '3');
	RegisterHotKey(wmInfo.info.win.window, static_cast<int32>(HotkeyID::COMMAND_VIEWPORT_4), MOD_CONTROL | MOD_ALT, '4');
	RegisterHotKey(wmInfo.info.win.window, static_cast<int32>(HotkeyID::FLIP_TEXCOORDS), MOD_CONTROL | MOD_ALT, '6');
	RegisterHotKey(wmInfo.info.win.window, static_cast<int32>(HotkeyID::TOGGLE_STREAM_TYPE), MOD_CONTROL | MOD_ALT, '7');
	RegisterHotKey(wmInfo.info.win.window, static_cast<int32>(HotkeyID::COMMAND_REPLICATED_VIEWPORT), MOD_CONTROL | MOD_ALT, '0');
	RegisterHotKey(wmInfo.info.win.window, static_cast<int32>(HotkeyID::SELECT_POINT), MOD_CONTROL | MOD_ALT, 'X');
}

void StreamManager::InitializeUIVertexArrays()
{
	// UI Texcoords
	sUIPositionArray[0] = -UI_X_OFFSET;
	sUIPositionArray[1] = UI_Y_OFFSET;
	sUIPositionArray[2] = -UI_X_OFFSET;
	sUIPositionArray[3] = UI_Y_OFFSET + UI_HEIGHT;
	sUIPositionArray[4] = UI_X_OFFSET;
	sUIPositionArray[5] = UI_Y_OFFSET;
	sUIPositionArray[6] = UI_X_OFFSET;
	sUIPositionArray[7] = UI_Y_OFFSET + UI_HEIGHT;

	sUITexcoordArray[0] = 0.0f;
	sUITexcoordArray[1] = 0.0f;
	sUITexcoordArray[2] = 0.0f;
	sUITexcoordArray[3] = 1.0f;
	sUITexcoordArray[4] = 1.0f;
	sUITexcoordArray[5] = 0.0f;
	sUITexcoordArray[6] = 1.0f;
	sUITexcoordArray[7] = 1.0f;
}