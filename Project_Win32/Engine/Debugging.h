#pragma once
#include "stdafx.h"

class Debugging
{
private:
	static Debugging *instance;
	unsigned int numTriangle;
	unsigned short numDrawCall;
	float *quadVertices;
	GLuint quadVAO, quadVBO;
public:
	static Debugging *getInstance();
	void addNumTriangle(unsigned int count);
	unsigned int getNumTriangle();
	void addDrawCall(unsigned short count);
	unsigned short getNumDrawCall();
	void resetCount();
	void DrawTex(GLuint TexId, const char *shadername = "debugShader");
	Debugging();
	~Debugging();
};
namespace QHEngine
{
	void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
	void DrawArrays(GLenum mode, GLint first, GLsizei count);
};