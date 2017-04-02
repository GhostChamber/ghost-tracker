#pragma once

#include <winsock.h>
#include "Enums.h"

class GestureListener
{

public:

	GestureListener();
	~GestureListener();

	void Initialize();

	void OpenListenServer();

	void Update();

	void SetStreamManager(class StreamManager* streamManager);

	void CloseListenServer();

private:

	void AcceptConnection();

	void ReadGestureData();

	void SetSocketBlocking(SOCKET& sock, bool blocking);

	class StreamManager* mStreamManager;

	SOCKET mListenSocket;
	SOCKET mConnectionSocket;

	ListenerState mState;
};