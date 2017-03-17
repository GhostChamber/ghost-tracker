#include "InputState.h"
#include <Windows.h>
#include <stdio.h>

InputState::InputState()
{
	memset(mKeys, 0, NUM_KEYS);
	memset(mPreviousKeys, 0, NUM_KEYS);
}

void InputState::Update()
{
	memcpy(mPreviousKeys, mKeys, NUM_KEYS);
	GetKeyboardState(mKeys);

	if (mKeys['5'] != 0)
	{
		printf("5 DOWN!\n");
	}
}

bool InputState::IsDown(int32 key) const
{
	if (key >= 0 &&
		key < NUM_KEYS)
	{
		return (mKeys[key] != 0);
	}

	return false;
}

bool InputState::JustDown(int32 key) const
{
	if (key >= 0 &&
		key < NUM_KEYS &&
		mKeys[key] != 0 &&
		mPreviousKeys[key] == 0)
	{
		return true;
	}

	return false;
}
