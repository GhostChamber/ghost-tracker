#include "MsgMouse.h"

MsgMouse::MsgMouse()
{

}

void MsgMouse::Read(char* pBuffer)
{
	mDeltaX = *reinterpret_cast<float*>(pBuffer);
	mDeltaY = *reinterpret_cast<float*>(pBuffer + 4);
	mLeftButtonDown = *reinterpret_cast<uint8*>(pBuffer + 8);
	mLeftButtonDown = *reinterpret_cast<uint8*>(pBuffer + 9);
	mLeftButtonDown = *reinterpret_cast<uint8*>(pBuffer + 10);
}

void MsgMouse::Write(char* pBuffer)
{
	
}

int MsgMouse::Size()
{
	return 0;
}

void MsgMouse::Clear()
{

}
