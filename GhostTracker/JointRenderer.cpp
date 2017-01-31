#include "JointRenderer.h"
#include "Meshes.h"
#include "SphereMesh.h"
#include "Matrix.h"
#include "Shaders.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static float sVertices[] = { -0.5f, 0.5f, 0.0f, 
						0.0f, -0.5f, 0.0f,
						0.5f, 0.5f, 0.0f};

JointRenderer::JointRenderer() :
	mMesh(0U)
{
	mColor[0] = 0.0f;
	mColor[1] = 1.0f;
	mColor[2] = 0.0f;
	mColor[3] = 1.0f;

	mPosition[0] = 0.0f;
	mPosition[1] = 0.0f;
	mPosition[2] = 0.0f;
}

JointRenderer::~JointRenderer()
{
	
}

void JointRenderer::UpdateFromJoint(Joint& joint)
{
	CameraSpacePoint point = joint.Position;
	SetPosition(point.X, point.Y, point.Z);
	mPosition[0] = point.X;
	mPosition[1] = point.Y;
	mPosition[2] = point.Z;
}

void JointRenderer::SetMeshType(unsigned int mesh)
{
	mMesh = mesh;
}

void JointRenderer::SetPosition(float x,
	float y,
	float z)
{
	mPosition[0] = x;
	mPosition[1] = y;
	mPosition[2] = z;
}

void JointRenderer::SetColor(float r,
	float g,
	float b,
	float a)
{
	mColor[0] = r;
	mColor[1] = g;
	mColor[2] = b;
	mColor[3] = a;
}

void JointRenderer::Render()
{
	GLuint hProg = GetShaderProgram(SHADER_COLOR_MESH);
	
	//glBindBuffer(GL_ARRAY_BUFFER, GetMeshVBOHandle(mMesh));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	Matrix model;
	model.Translate(mPosition[0], mPosition[1], -1.0f * mPosition[2]);
	model.Scale(0.2f, 0.2f, 0.2f);
	Matrix view;
	Matrix projection;
	projection.Frustum(-0.0577f, 0.0577f, -0.03639f, 0.03639f, 0.1f, 1024.0f);
	Matrix mvp = projection * view * model;
	
	glEnableVertexAttribArray(hPosition);
	glVertexAttribPointer(hPosition, 3, GL_FLOAT, GL_FALSE, 0, sVertices);

	 
	glUniform4fv(hColor, 1, mColor);
	//glUniformMatrix4fv(hMatrixMVP, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(hMatrixMVP, 1, GL_FALSE, mvp.GetArray() /*glm::value_ptr(mvp)*/);

	glDrawArrays(GL_TRIANGLES, 0, 3 /** GetMeshFaceCount(mMesh)*/);
}

void JointRenderer::SetRenderingState()
{
	//static GLuint vao = 0;
	//if (vao == 0)
	//{
	//	glGenVertexArrays(1, &vao);
	//}

	//glBindVertexArray(vao);
	glDisable(GL_DEPTH_TEST);
}

void JointRenderer::ClearRenderingState()
{
	glEnable(GL_DEPTH_TEST);
	//glBindVertexArray(0);
}
