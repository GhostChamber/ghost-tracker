#pragma once

#include "Types.h"
#include "Constants.h"

struct InputState
{
	InputState();
	void Update();

	bool IsDown(int32 key) const;
	bool JustDown(int32 key) const;

	uint8 mKeys[NUM_KEYS];
	uint8 mPreviousKeys[NUM_KEYS];
};