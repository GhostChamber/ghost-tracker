#pragma once

#include "Message.h"
#include "Types.h"

class MsgMouse : public Message
{
public:
	MsgMouse();

	void Read(char* pBuffer);
	void Write(char* pBuffer);
	int Size();
	void Clear();

	float mDeltaX;
	float mDeltaY;
	uint8 mLeftButtonDown;
	uint8 mRightButtonDown;
	uint8 mMiddleButtonDown;
};

