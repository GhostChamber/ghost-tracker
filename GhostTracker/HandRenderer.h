#pragma once

#include "Meshes.h"
#include "OpenGL.h"
#include <Kinect.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum HandSideEnum
{
	HAND_LEFT,
	HAND_RIGHT
};

enum HandGestureEnum
{
	
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

	void UpdateGestures();
	void UpdatePosition(CameraSpacePoint handPoint);
	void UpdateRotation(CameraSpacePoint wristPoint, CameraSpacePoint handPoint);
	void UpdateClamp(CameraSpacePoint thumbPoint, CameraSpacePoint tipPoint);

	unsigned int mHandSide;
	unsigned int mMesh;
	bool mClamped;
	float mColor[4];
	glm::vec3 mPosition;
	glm::vec3 mDirection;
};