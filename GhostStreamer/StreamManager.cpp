#include "StreamManager.h"
#include "Constants.h"
#include "Enums.h"
#include "ViewportCapturer.h"
#include <Windows.h>
#include <assert.h>
#include <stdio.h>
#include <SDL_syswm.h>
#include "Shaders.h"



StreamManager::StreamManager() :
	mStreamType(StreamType::REPLICATED),
	mCommandState(CommandState::LISTENING),
	mPoint1X(0),
	mPoint1Y(0),
	mPoint2X(0),
	mPoint2Y(0),
	mCurrentGesture(GhostGesture::NONE)
{
	for (int32 i = 0; i < NUM_DISPLAY_QUADRANTS; i++)
	{
		mIndividualCapturers[i].SetQuadrant(i);
	}

	ViewportCapturer::InitializeMeshArrays();
	GestureIcon::InitializeVertexArrays();
	GestureIcon::LoadTextures();

	mGestureIcon.SetStreamManager(this);
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

	ViewportCapturer::ClearRenderingState();

	GestureIcon::SetRenderingState();
	mGestureIcon.Render();
	GestureIcon::ClearRenderingState();
}

GhostGesture StreamManager::GetCurrentGesture() const
{
	return mCurrentGesture;
}

void StreamManager::SetCurrentGesture(GhostGesture gesture)
{
	mCurrentGesture = gesture;
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
