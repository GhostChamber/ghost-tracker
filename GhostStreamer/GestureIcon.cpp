
#include "GestureIcon.h"
#include "StreamManager.h"
#include "Constants.h"
#include "Shaders.h"
#include "Matrix.h"

#include "SDL_image.h"

float GestureIcon::sPositionArray[2 * 4] = { 0 };
float GestureIcon::sTexcoordArray[2 * 4] = { 0 };

GLuint GestureIcon::sTextureGrab = 0U;
GLuint GestureIcon::sTextureZoom = 0U;
GLuint GestureIcon::sTextureOrbit = 0U;


GestureIcon::GestureIcon() :
	mStreamManager(nullptr)
{

}

void GestureIcon::InitializeVertexArrays()
{
	sPositionArray[0] = -UI_X_OFFSET;
	sPositionArray[1] = UI_Y_OFFSET;
	sPositionArray[2] = -UI_X_OFFSET;
	sPositionArray[3] = UI_Y_OFFSET + UI_HEIGHT;
	sPositionArray[4] = UI_X_OFFSET;
	sPositionArray[5] = UI_Y_OFFSET;
	sPositionArray[6] = UI_X_OFFSET;
	sPositionArray[7] = UI_Y_OFFSET + UI_HEIGHT;

	sTexcoordArray[0] = 1.0f;
	sTexcoordArray[1] = 1.0f;
	sTexcoordArray[2] = 1.0f;
	sTexcoordArray[3] = 0.0f;
	sTexcoordArray[4] = 0.0f;
	sTexcoordArray[5] = 1.0f;
	sTexcoordArray[6] = 0.0f;
	sTexcoordArray[7] = 0.0f;
}

void GestureIcon::LoadTextures()
{
	SDL_Surface* grabSurface = IMG_Load("GrabIcon.png");
	SDL_Surface* zoomSurface = IMG_Load("ZoomIcon.png");
	SDL_Surface* orbitSurface = IMG_Load("OrbitIcon.png");

	glGenTextures(1, &sTextureGrab);
	glGenTextures(1, &sTextureZoom);
	glGenTextures(1, &sTextureOrbit);

	glBindTexture(GL_TEXTURE_2D, sTextureGrab);
	glTexImage2D(GL_TEXTURE_2D, 
				 0,
				 GL_RGBA,
				 ICON_WIDTH,
				 ICON_HEIGHT,
				 0,
				 GL_RGBA,
				 GL_UNSIGNED_BYTE,
				 grabSurface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, sTextureZoom);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGBA,
				 ICON_WIDTH,
				 ICON_HEIGHT,
				 0,
				 GL_RGBA,
				 GL_UNSIGNED_BYTE,
				 zoomSurface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, sTextureOrbit);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGBA,
				 ICON_WIDTH,
				 ICON_HEIGHT,
				 0,
				 GL_RGBA,
				 GL_UNSIGNED_BYTE,
				 orbitSurface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(grabSurface);
	SDL_FreeSurface(zoomSurface);
	SDL_FreeSurface(orbitSurface);
}

void GestureIcon::Render()
{
	GLuint hProg = GetShaderProgram(SHADER_ICON);

	GLint hPosition = glGetAttribLocation(hProg, "aPosition");
	GLint hTexcoord = glGetAttribLocation(hProg, "aTexcoord");

	float color[4] = { 1.0f };

	switch (mStreamManager->GetCurrentGesture())
	{
	case GhostGesture::NONE:
		color[0] = 0.0f;
		color[1] = 0.0f;
		color[2] = 0.0f;
		color[3] = 0.0f;
		glBindTexture(GL_TEXTURE_2D, 0);
		return;
		break;
	case GhostGesture::GRAB:
		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 1.0f;
		color[3] = 1.0f;
		glBindTexture(GL_TEXTURE_2D, sTextureGrab);
		break;
	case GhostGesture::ZOOM:
		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 1.0f;
		color[3] = 1.0f;
		glBindTexture(GL_TEXTURE_2D, sTextureZoom);
		break;
	case GhostGesture::ORBIT:
		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 1.0f;
		color[3] = 1.0f;
		glBindTexture(GL_TEXTURE_2D, sTextureOrbit);
		break;
	}

	glEnableVertexAttribArray(hPosition);
	glEnableVertexAttribArray(hTexcoord);

	glVertexAttribPointer(hPosition,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		sPositionArray);
	glVertexAttribPointer(hTexcoord,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		sTexcoordArray);

	GLint hTexture = glGetUniformLocation(hProg, "uTexture");
	GLint hMatrix = glGetUniformLocation(hProg, "uMatrix");
	//GLint hWidthScale = glGetUniformLocation(hProg, "uWidthScale");
	//GLint hHeightScale = glGetUniformLocation(hProg, "uHeightScale");
	//GLint hTextureMode = glGetUniformLocation(hProg, "uTextureMode");
	GLint hColor = glGetUniformLocation(hProg, "uColor");

	glUniform1i(hTexture, 0);
	//glUniform1f(hWidthScale, 1.0f);
	//glUniform1f(hHeightScale, 1.0f);
	//glUniform1i(hTextureMode, 1);
	glUniform4fv(hColor, 1, color);

	Matrix matrix;
	for (int i = 0; i < 4; i++)
	{
		matrix.Rotate(90.0f, 0.0f, 0.0f, 1.0f);
		glUniformMatrix4fv(hMatrix, 1, GL_FALSE, matrix.GetArray());

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void GestureIcon::SetStreamManager(class StreamManager* manager)
{
	mStreamManager = manager;
}

void GestureIcon::SetRenderingState()
{
	glUseProgram(GetShaderProgram(SHADER_ICON));
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
}

void GestureIcon::ClearRenderingState()
{

}