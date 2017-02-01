#include "HandRenderer.h"
#include "Meshes.h"
#include "SphereMesh.h"
#include "Matrix.h"
#include "Shaders.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define CLAMP_DISTANCE_THRESHOLD 0.007f

static float sVertices[] = { -0.5f, 0.5f, 0.0f, 
						0.0f, -0.5f, 0.0f,
						0.5f, 0.5f, 0.0f};

static float sClampColor[] = {1.0f, 1.0f, 1.0f, 1.0f};

HandRenderer::HandRenderer() :
	mHandSide(HAND_LEFT),
	mMesh(ARROW)
{
	mColor[0] = 0.0f;
	mColor[1] = 1.0f;
	mColor[2] = 0.0f;
	mColor[3] = 1.0f;

	mPosition.x = 0.0f;
	mPosition.y = 0.0f;
	mPosition.z = 0.0f;
}

HandRenderer::~HandRenderer()
{
	
}

void HandRenderer::UpdateFromBodyData(Joint* joints)
{
	CameraSpacePoint wristPoint;
	CameraSpacePoint handPoint;
	CameraSpacePoint thumbPoint;
	CameraSpacePoint tipPoint;

	if (mHandSide == HAND_LEFT)
	{
		wristPoint = joints[JointType_WristLeft].Position;
		handPoint = joints[JointType_HandLeft].Position;
		thumbPoint = joints[JointType_ThumbLeft].Position;
		tipPoint = joints[JointType_HandTipLeft].Position;
		//tipPoint = joints[JointType_HipLeft].Position;
	}
	else
	{
		wristPoint = joints[JointType_WristRight].Position;
		handPoint = joints[JointType_HandRight].Position;
		thumbPoint = joints[JointType_ThumbRight].Position;
		tipPoint = joints[JointType_HandTipRight].Position;
		//tipPoint = joints[JointType_HipRight].Position;
	}

	UpdatePosition(handPoint);
	UpdateRotation(wristPoint, handPoint);
	UpdateClamp(thumbPoint, tipPoint);
}

void HandRenderer::SetHandSide(unsigned int handSide)
{
	mHandSide = handSide;
}

unsigned int HandRenderer::GetHandSide()
{
	return mHandSide;
}

void HandRenderer::SetMeshType(unsigned int mesh)
{
	mMesh = mesh;
}

void HandRenderer::SetPosition(float x,
	float y,
	float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void HandRenderer::SetColor(float r,
	float g,
	float b,
	float a)
{
	mColor[0] = r;
	mColor[1] = g;
	mColor[2] = b;
	mColor[3] = a;
}

void HandRenderer::Render()
{
	GLuint hProg = GetShaderProgram(SHADER_COLOR_MESH);
	
	glBindBuffer(GL_ARRAY_BUFFER, GetMeshVBOHandle(mMesh));
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(hProg);
	
	GLint hPosition = glGetAttribLocation(hProg, "aPosition");
	GLint hColor = glGetUniformLocation(hProg, "uColor");
	GLint hMatrixMVP = glGetUniformLocation(hProg, "uMatrixMVP");

	//glm::mat4 mvp;
	//glm::mat4 model;
	//glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
	//glm::mat4 view;
	//glm::lookAt(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 proj = glm::perspective(glm::radians(80.0f), 1280.0f / 720.0f, 1.0f, 20.0f);
	//mvp = proj * view * model;
	//glm::perspectiveFov(45.0f, 0.0577f, 0.03639f, 0.1f, 1024.0f);

	//glm::mat4 glmLookAt = glm::lookAt(mPosition, mPosition + mDirection, glm::vec3(0.0f, 0.0f, -1.0f));
	//Matrix lookAt;
	//lookAt.Load(value_ptr(glmLookAt));

	Matrix model;
	model.Translate(mPosition[0], mPosition[1], -1.0f * mPosition[2]);
	model.Scale(0.2f, 0.2f, 0.2f);
	//model = model * lookAt;
	//model = lookAt * model;

	Matrix view;
	Matrix projection;
	projection.Frustum(-0.0577f, 0.0577f, -0.03639f, 0.03639f, 0.1f, 1024.0f);
	Matrix mvp = projection * view * model;
	
	glEnableVertexAttribArray(hPosition);
	glVertexAttribPointer(hPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0 /*sVertices*/);

	if (mClamped)
	{
		glUniform4fv(hColor, 1, sClampColor);
	}
	else
	{
		glUniform4fv(hColor, 1, mColor);
	}
	
	//glUniformMatrix4fv(hMatrixMVP, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(hMatrixMVP, 1, GL_FALSE, mvp.GetArray() /*glm::value_ptr(mvp)*/);

	glDrawArrays(GL_TRIANGLES, 0, 3 * GetMeshFaceCount(mMesh));
}

void HandRenderer::SetRenderingState()
{
	//static GLuint vao = 0;
	//if (vao == 0)
	//{
	//	glGenVertexArrays(1, &vao);
	//}

	//glBindVertexArray(vao);
	glEnable(GL_DEPTH_TEST);
}

void HandRenderer::ClearRenderingState()
{
	glDisable(GL_DEPTH_TEST);
	//glBindVertexArray(0);
}

void HandRenderer::UpdatePosition(CameraSpacePoint handPoint)
{
	SetPosition(handPoint.X, handPoint.Y, handPoint.Z);
}

void HandRenderer::UpdateRotation(CameraSpacePoint wristPoint, CameraSpacePoint handPoint)
{
	mDirection.x = handPoint.X - wristPoint.X;
	mDirection.y = handPoint.Y - wristPoint.Y;
	mDirection.z = handPoint.Z - wristPoint.Z;
}

void HandRenderer::UpdateClamp(CameraSpacePoint thumbPoint, CameraSpacePoint tipPoint)
{
	// Check if crab claw is clamped.
	float clampDistanceSquared = (thumbPoint.X - tipPoint.X) * (thumbPoint.X - tipPoint.X) +
		(thumbPoint.Y - tipPoint.Y) * (thumbPoint.Y - tipPoint.Y) +
		(thumbPoint.Z - tipPoint.Z) * (thumbPoint.Z - tipPoint.Z);

	mClamped = clampDistanceSquared < CLAMP_DISTANCE_THRESHOLD;
}
