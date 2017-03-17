#pragma once

enum MessageEnum
{
	MSG_INVALID = -1,
	MSG_MOUSE = 0,
	NUM_MESSAGES
};

enum MessageSizes
{
	MSG_MOUSE_SIZE = 11
};

class Message
{
public:
	Message();
	int mID;

	virtual void Read(char* pBuffer) = 0;
	virtual void Write(char* pBuffer) = 0;
	virtual int Size() = 0;
	virtual void Clear() = 0;
};
