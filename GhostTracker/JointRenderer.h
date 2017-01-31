#pragma once

#include "Meshes.h"
#include "OpenGL.h"
#include <Kinect.h>

class JointRenderer
{
public:

	JointRenderer();
	virtual ~JointRenderer();

	void SetMeshType(unsigned int mesh);

	void UpdateFromJoint(Joint& joint);

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

	unsigned int mMesh;
	float mColor[4];
	float mPosition[3];
};