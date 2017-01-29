#include <cmath>
#include <cstdio>
#include <Windows.h>
#include <Ole2.h>

#include "Kinect.h"

const int DEPTH_WIDTH = 512;
const int DEPTH_HEIGHT = 424;
const int COLOR_WIDTH = 1920;
const int COLOR_HEIGHT = 1080;

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

int InitKinect()
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
		lh = joints[JointType_WristLeft].Position;
		const CameraSpacePoint& le = joints[JointType_ElbowLeft].Position;;
		const CameraSpacePoint& ls = joints[JointType_ShoulderLeft].Position;;
		rh = joints[JointType_WristRight].Position;;
		const CameraSpacePoint& re = joints[JointType_ElbowRight].Position;;
		const CameraSpacePoint& rs = joints[JointType_ShoulderRight].Position;;
	}
}

int main(int argc, char* argv[])
{
	if (InitKinect() == 0)
	{
		return 1;
	}

	// Main loop
	while (GetAsyncKeyState(VK_ESCAPE) == 0)
	{
		UpdateBodyData();

		if (bodyTracked)
		{
			if (lh.Y > rh.Y)
			{
				printf("LEFT                     \n");
			}
			else
			{
				printf("                    RIGHT\n");
			}
		}
		else
		{
			printf("===========================\n");
		}
	}

	return 0;
}
