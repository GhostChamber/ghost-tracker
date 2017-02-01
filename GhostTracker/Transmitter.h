#pragma once

enum MessageEnum
{
	MSG_MOUSE = 0,
	NUM_MESSAGES 
};

class Transmitter
{
public:

	static Transmitter* GetInstance();
	static void DestroyInstance();

	void SendMessage()
private:


	Transmitter();
};