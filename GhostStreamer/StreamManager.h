#pragma once

#include "ViewportCapturer.h"
#include "InputState.h"
#include "Types.h"
#include "Constants.h"
#include "Enums.h"
#include "GestureIcon.h"
#include <string>

class StreamManager
{
public:

	StreamManager();
	~StreamManager();

	void Update();

	void Render();

	void ActivateHotkey(WPARAM wparam, LPARAM lparam);

	void Initialize(SDL_Window* window);

	void SetupHotkeys(SDL_Window* window) const;

	void SetCurrentGesture(GhostGesture gesture);

	GhostGesture GetCurrentGesture() const;

private:

	static const LPCSTR sAutoCADWindowNameClass;

	ViewportCapturer mReplicatedCapturer;
	ViewportCapturer mIndividualCapturers[NUM_DISPLAY_QUADRANTS];

	StreamType mStreamType;

	InputState mInputState;

	CommandState mCommandState;

	ViewportCapturer* mCommandedCapturer;
	int32 mPoint1X;
	int32 mPoint1Y;
	int32 mPoint2X;
	int32 mPoint2Y;

	GestureIcon mGestureIcon;

	GhostGesture mCurrentGesture;
};
