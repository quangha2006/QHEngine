#include "Debugging.h"
#include <iostream>
#include "ShaderManager.h"

Debugging *Debugging::instance = NULL;

Debugging * Debugging::getInstance()
{
	if (instance == NULL)
		instance = new Debugging();
	return instance;
}

void Debugging::addNumTriangle(unsigned int count)
{
	numTriangle += count;
}

unsigned int Debugging::getNumTriangle()
{
	return numTriangle;
}

void Debugging::addDrawCall(unsigned short count)
{
	numDrawCall += count;
}

unsigned short Debugging::getNumDrawCall()
{
	return numDrawCall;
}

void Debugging::resetCount()
{
	numTriangle = 0;
	numDrawCall = 0;
}

void Debugging::DrawTex(GLuint TexId)
{

	//Debug
	glEnable(GL_DEPTH_TEST);
	ShaderManager::getInstance()->setUseProgram("screenShader");
	glBindVertexArray(quadVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexId);	// use the color attachment texture as the texture of the quad plane

	ShaderManager::getInstance()->setInt("depthMap", 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glDisable(GL_DEPTH_TEST);
	//End debug
}

Debugging::Debugging()
{
	numTriangle = 0;
	numDrawCall = 0;
	quadVertices = new float[5 * 4]{
		// positions   // texCoords
		0.2f,  1.0f, 0.0f, 0.0f, 1.0f,
		0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f,  0.2f, 0.0f, 1.0f, 0.0f
	};
	glBindVertexArray(0);
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);
}


Debugging::~Debugging()
{
	delete[] quadVertices;
	glDeleteBuffers(1, &quadVBO);
}

void QHEngine::DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	glDrawElements(mode, count, type, indices);
	Debugging::getInstance()->addDrawCall(1);
	Debugging::getInstance()->addNumTriangle(count/3);
}

void QHEngine::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
	glDrawArrays(mode, first, count);
	Debugging::getInstance()->addDrawCall(1);
	Debugging::getInstance()->addNumTriangle(count / 3);
}
