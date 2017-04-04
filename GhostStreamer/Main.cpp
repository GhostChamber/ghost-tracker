#include <cmath>
#include <cstdio>
#include <Windows.h>
#include <Ole2.h>

#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>
#include <string.h>
#include <stdio.h>

#include "Shaders.h"
#include "StreamManager.h"
#include "GestureListener.h"
#include <SDL_events.h>
#include <SDL_syswm.h>

SDL_GLContext context;
SDL_Window* window = nullptr;

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 1080

void Close()
{
	//Deallocate program
	UnloadShaders();

	//Destroy window	
	SDL_DestroyWindow(window);
	window = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void InitializeGraphics()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be initialized.\n");
		return;
	}

	//Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window = SDL_CreateWindow("Ghost Tracker", 
								SDL_WINDOWPOS_UNDEFINED, 
								SDL_WINDOWPOS_UNDEFINED, 
								SCREEN_WIDTH,
								SCREEN_HEIGHT, 
								SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
	if (window == nullptr)
	{
		printf("Window could not be created.\n");
		return;
	}

	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

	context = SDL_GL_CreateContext(window);
	if (context == nullptr)
	{
		printf("OpenGL context could not be created.\n");
		return;
	}

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		printf("Error initializing GLEW.\n");
	}

	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		printf("Unable to set vertical sync.\n");
	}

	LoadShaders();
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
}

void ProcessWindowsEvent(SDL_SysWMEvent event, StreamManager& streamer)
{
	SDL_SysWMmsg* message = event.msg;

	if (message != nullptr &&
		message->msg.win.msg == WM_HOTKEY)
	{
		streamer.ActivateHotkey(message->msg.win.wParam, message->msg.win.lParam);
	}
}

int main(int argc, char* argv[])
{
	InitializeGraphics();
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

	bool quit = false;
	SDL_Event e;

	StreamManager streamer;
	streamer.Initialize(window);


	GestureListener gestureListener;
	gestureListener.Initialize();
	gestureListener.OpenListenServer();
	gestureListener.SetStreamManager(&streamer);


	// Main loop
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			if (e.type == SDL_SYSWMEVENT)
			{
				ProcessWindowsEvent(e.syswm, streamer);
			}
		}

		gestureListener.Update();
		streamer.Update();
		streamer.Render();
		//Render();

		SDL_GL_SwapWindow(window);
	}

	Close();

	return 0;
}
