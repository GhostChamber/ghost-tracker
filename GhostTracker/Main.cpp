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

#include "Kinect.h"
#include "Shaders.h"
#include "HandRenderer.h"

const int DEPTH_WIDTH = 512;
const int DEPTH_HEIGHT = 424;
const int COLOR_WIDTH = 1920;
const int COLOR_HEIGHT = 1080;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

void UpdateBodyData();

// Intermediate Buffers
unsigned char rgbimage[COLOR_WIDTH * COLOR_HEIGHT * 4];
ColorSpacePoint depth2rgb[DEPTH_WIDTH * DEPTH_HEIGHT];
CameraSpacePoint depth2xyz[DEPTH_WIDTH * DEPTH_WIDTH];

// Body tracking variables
BOOLEAN bodyTracked;
Joint joints[JointType_Count];

// Kinect Variables
IKinectSensor* sensor;
IMultiSourceFrameReader* reader;
ICoordinateMapper* mapper;

CameraSpacePoint lh;
CameraSpacePoint rh;

SDL_GLContext context;
SDL_Window* window = nullptr;

HandRenderer leftHand;
HandRenderer rightHand;

void Close()
{
	//Deallocate program
	UnloadShaders();
	UnloadMeshes();

	//Destroy window	
	SDL_DestroyWindow(window);
	window = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}

int InitializeKinect()
{
	if (FAILED(GetDefaultKinectSensor(&sensor)))
	{
		return 0;
	}

	if (sensor)
	{
		sensor->get_CoordinateMapper(&mapper);

		sensor->Open();
		sensor->OpenMultiSourceFrameReader(FrameSourceTypes::FrameSourceTypes_Depth | 
			FrameSourceTypes::FrameSourceTypes_Color | 
			FrameSourceTypes::FrameSourceTypes_Body,
			&reader);

		return (reader != nullptr);
	}
	else
	{
		return 0;
	}
}

void GetDepthData(IMultiSourceFrame* frame, unsigned char* dest)
{
	IDepthFrame* depthframe;
	IDepthFrameReference* frameref = NULL;
	frame->get_DepthFrameReference(&frameref);
	frameref->AcquireFrame(&depthframe);

	if (frameref)
	{
		frameref->Release();
	}

	if (!depthframe)
	{
		return;
	}

	// Get data from frame
	unsigned int size;
	unsigned short* buffer;
	depthframe->AccessUnderlyingBuffer(&size, &buffer);

	// Write vertex coordinates
	mapper->MapDepthFrameToCameraSpace(size, buffer, DEPTH_WIDTH * DEPTH_HEIGHT, depth2xyz);
	float* fdest = (float*)dest;
	for (unsigned int i = 0; i < size; i++)
	{
		*fdest++ = depth2xyz[i].X;
		*fdest++ = depth2xyz[i].Y;
		*fdest++ = depth2xyz[i].Z;
	}

	// Fill in depth2rgb map
	mapper->MapDepthFrameToColorSpace(size, buffer, DEPTH_WIDTH * DEPTH_HEIGHT, depth2rgb);
	if (depthframe != nullptr)
	{
		depthframe->Release();
	}
}

void GetRgbData(IMultiSourceFrame* frame, unsigned char* dest)
{
	IColorFrame* colorframe;
	IColorFrameReference* frameref = NULL;
	frame->get_ColorFrameReference(&frameref);
	frameref->AcquireFrame(&colorframe);
	if (frameref) frameref->Release();

	if (colorframe == nullptr)
	{
		return;
	}

	// Get data from frame
	colorframe->CopyConvertedFrameDataToArray(COLOR_WIDTH * COLOR_HEIGHT * 4, rgbimage, ColorImageFormat_Rgba);

	// Write color array for vertices
	ColorSpacePoint* p = depth2rgb;
	float* fdest = (float*)dest;
	for (int i = 0; i < DEPTH_HEIGHT; i++)
	{
		for (int j = 0; j < DEPTH_WIDTH; j++)
		{
			if (p->X < 0 || p->Y < 0 || p->X > COLOR_WIDTH || p->Y > COLOR_HEIGHT)
			{
				*fdest++ = 0;
				*fdest++ = 0;
				*fdest++ = 0;
			}
			else
			{
				int idx = (int)p->X + COLOR_WIDTH*(int)p->Y;
				*fdest++ = rgbimage[4 * idx + 0] / 255.0f;
				*fdest++ = rgbimage[4 * idx + 1] / 255.0f;
				*fdest++ = rgbimage[4 * idx + 2] / 255.0f;
			}

			// Don't copy alpha channel
			p++;
		}
	}

	if (colorframe)
	{
		colorframe->Release();
	}
}

void GetBodyData(IMultiSourceFrame* frame)
{
	IBodyFrame* bodyframe;
	IBodyFrameReference* frameref = NULL;
	frame->get_BodyFrameReference(&frameref);
	frameref->AcquireFrame(&bodyframe);
	if (frameref) frameref->Release();

	if (bodyframe == nullptr)
	{
		return;
	}

	IBody* body[BODY_COUNT] = { 0 };
	bodyframe->GetAndRefreshBodyData(BODY_COUNT, body);
	for (int i = 0; i < BODY_COUNT; i++)
	{
		body[i]->get_IsTracked(&bodyTracked);
		if (bodyTracked)
		{
			body[i]->GetJoints(JointType_Count, joints);
			break;
		}
	}

	if (bodyframe != nullptr)
	{
		bodyframe->Release();
	}
}

void GetKinectData()
{
	IMultiSourceFrame* frame = nullptr;

	if (SUCCEEDED(reader->AcquireLatestFrame(&frame)))
	{
		GetBodyData(frame);
	}

	if (frame != nullptr)
	{
		frame->Release();
	}
}

void UpdateBodyData()
{
	GetKinectData();

	if (bodyTracked)
	{
		// Draw some arms
		leftHand.UpdateFromBodyData(joints);
		rightHand.UpdateFromBodyData(joints);
	}
	else
	{
		leftHand.SetPosition(-2.0f, 0.0f, -5.0f);
		rightHand.SetPosition(2.0f, 0.0f, -5.0f);
	}
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Joint rendering
	HandRenderer::SetRenderingState();
	leftHand.Render();
	rightHand.Render();
	HandRenderer::ClearRenderingState();
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
								SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		printf("Window could not be created.\n");
		return;
	}

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
	LoadMeshes();
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
}

int main(int argc, char* argv[])
{
	InitializeGraphics();

	if (InitializeKinect() == 0)
	{
		return 1;
	}

	bool quit = false;
	SDL_Event e;

	leftHand.SetHandSide(HAND_LEFT);
	rightHand.SetHandSide(HAND_RIGHT);
	leftHand.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
	rightHand.SetColor(0.0f, 0.0f, 1.0f, 1.0f);

	// Main loop
	while (!quit)
	{
		UpdateBodyData();

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		Render();

		SDL_GL_SwapWindow(window);
	}

	Close();

	return 0;
}
