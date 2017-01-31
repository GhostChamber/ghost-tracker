#pragma once

#include "Meshes.h"
#include "OpenGL.h"
#include <Kinect.h>

enum HandSideEnum
{
	HAND_LEFT,
	HAND_RIGHT
};

class HandRenderer
{
public:

	HandRenderer();
	virtual ~HandRenderer();

	void SetHandSide(unsigned int handSide);

	unsigned int GetHandSide();

	void SetMeshType(unsigned int mesh);

	bool IsClamped();

	void UpdateFromBodyData(Joint* joints);

	void SetPosition(float x,
					 float y,
					 float z);

	void SetColor(float r,
				  float g,
				  float b,
				  float a);

	void Render();

	static void SetRenderingState();

	static void ClearRenderingState();

private:

	unsigned int mHandSide;
	unsigned int mMesh;
	bool mClamped;
	float mColor[4];
	float mPosition[3];
};