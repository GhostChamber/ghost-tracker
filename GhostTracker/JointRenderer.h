#pragma once

#include "OpenGL.h"

class JointRenderer
{
public:

	JointRenderer();
	virtual ~JointRenderer();

	void SetVertexPositionArray();

private:

	GLuint mVBOHandle;
	
};