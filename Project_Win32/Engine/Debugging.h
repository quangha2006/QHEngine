#ifndef __DEBUGGING_H__
#define __DEBUGGING_H__

#include "stdafx.h"
#include "Shader.h"
#include "Model.h"

class Debugging
{
private:
	static Debugging *instance;
	unsigned int mNumVertices;
	unsigned short numDrawCall;
	float *quadVertices;
	GLuint quadVAO, quadVBO;
	Shader mQuadDebug_shader;
	Model mBall_Model;
public:
	static Debugging *getInstance();
	void addNumVertices(unsigned int count);
	unsigned int getNumVertices();
	void addDrawCall(unsigned short count);
	unsigned short getNumDrawCall();
	void resetCount();
	void RenderTexture(GLuint TexId);
	void RenderBall(glm::vec3 pos);
	Debugging();
	~Debugging();
};
namespace QHEngine
{
	void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
	void DrawArrays(GLenum mode, GLint first, GLsizei count);
	void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount);
	void RenderDebugTexture(GLuint textureId);
};
#endif // !__DEBUGGING_H__