#include "GestureListener.h"
#include "StreamManager.h"
#include "stdio.h"

#include <winsock.h>

GestureListener::GestureListener() :
	mStreamManager(nullptr),
	mListenSocket(INVALID_SOCKET),
	mConnectionSocket(INVALID_SOCKET),
	mState(ListenerState::INACTIVE)
{

}

GestureListener::~GestureListener()
{
	CloseListenServer();
}

void GestureListener::Initialize()
{
	WSADATA wsadata;

	int nError = WSAStartup(0x0202, &wsadata);

	//Did something happen?
	if (nError)
	{
		printf("Error initializing winsock library.");
		return;
	}

	//Did we get the right Winsock version?
	if (wsadata.wVersion != 0x0202)
	{
		WSACleanup();
		return;
	}
}

void GestureListener::OpenListenServer()
{
	SOCKADDR_IN sinTarget;

	sinTarget.sin_family = AF_INET;
	sinTarget.sin_addr.s_addr = htonl(INADDR_ANY);
	sinTarget.sin_port = htons(LISTEN_SERVER_PORT);
	mListenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (mListenSocket == INVALID_SOCKET)
	{
		printf("Failed to create socket in Socket::Open().");
		return;
	}

	if (bind(mListenSocket, (LPSOCKADDR)&sinTarget, sizeof(sinTarget)) == SOCKET_ERROR)
	{
		printf("Socket could not be bound to specified port.");
		printf("Error code: %d\n", WSAGetLastError());
		return;
	}

	SetSocketBlocking(mListenSocket, false);

	mState = ListenerState::LISTENING;
}

void GestureListener::Update()
{
	if (mState == ListenerState::LISTENING)
	{
		AcceptConnection();
	}
	else if (mState == ListenerState::CONNECTED)
	{
		ReadGestureData();
	}
}

void GestureListener::SetStreamManager(StreamManager* streamManager)
{
	mStreamManager = streamManager;
}

void GestureListener::CloseListenServer()
{
	if (mConnectionSocket != INVALID_SOCKET)
	{
		closesocket(mConnectionSocket);
		mConnectionSocket = INVALID_SOCKET;
	}

	if (mListenSocket != INVALID_SOCKET)
	{
		closesocket(mListenSocket);
		mListenSocket = INVALID_SOCKET;
	}
}

void GestureListener::AcceptConnection()
{
	SOCKET newSock = 0;

	if (listen(mListenSocket, 1) == SOCKET_ERROR)
	{
		// No connections
		return;
	}

	newSock = accept(mListenSocket, 0, 0);

	if (newSock == INVALID_SOCKET)
	{
		// No incoming connections, so return 0.
		return;
	}

	printf("\nClient connected!\n");

	mConnectionSocket = newSock;
	mState = ListenerState::CONNECTED;
}

void GestureListener::ReadGestureData()
{
	char message;

	while (true)
	{
		int size = recv(mConnectionSocket, &message, 1, 0);

		if (size <= 0)
		{
			break;
		}

		printf("Message received!\n");

		if (mStreamManager == nullptr)
		{
			break;
		}

		switch (message)
		{

		case GhostMessage::NO_GESTURE:
			mStreamManager->SetCurrentGesture(GhostGesture::NONE);
			break;

		case GhostMessage::GRAB:
			mStreamManager->SetCurrentGesture(GhostGesture::GRAB);
			break;

		case GhostMessage::ZOOM:
			mStreamManager->SetCurrentGesture(GhostGesture::ZOOM);
			break;

		case GhostMessage::ORBIT:
			mStreamManager->SetCurrentGesture(GhostGesture::ORBIT);
			break;

		default:
			printf("Unknown message received: %d\n", static_cast<int>(message));
			break;

		}
	}
}

void GestureListener::SetSocketBlocking(SOCKET& sock, bool blocking)
{
	unsigned long lEnable = static_cast<unsigned long>(!blocking);

	if (ioctlsocket(sock, FIONBIO, &lEnable) == SOCKET_ERROR)
	{
		printf("Socket could not be set to blocking/nonblocking.");
	}
}