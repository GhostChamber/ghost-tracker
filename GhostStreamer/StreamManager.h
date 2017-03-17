#pragma once

#include "ViewportCapturer.h"
#include "InputState.h"
#include "Types.h"
#include "Constants.h"
#include "Enums.h"

class StreamManager
{
public:

	StreamManager();
	~StreamManager();

	void Update();

	void Render();

	void ActivateHotkey(WPARAM wparam, LPARAM lparam);

	void SetupHotkeys(SDL_Window* window) const;

private:

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
};