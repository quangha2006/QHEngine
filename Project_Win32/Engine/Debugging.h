#ifndef __DEBUGGING_H__
#define __DEBUGGING_H__

#include "stdafx.h"
#include "Shader.h"
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Debugging
{
private:
	static Debugging *instance;
	GLuint mNumVertices;
	unsigned short numDrawCall;
	float *quadVertices;
	GLuint quadVAO, quadVBO;
	Shader mQuadDebug_shader;
	// Ball Debug
	GLuint mNumVertices_ball;
	GLuint mNumIndices_ball;
	float* mVertices_ball;
	GLuint* mIndices_ball;
	GLuint mVBO_ball, mEBO_ball;
	Shader mBall_shader;

	bool mIsInitBall;
	void InitBallData();
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
	void EnablePolygonModeGLLine();
	void DisablePolygonModeGLLine();
};
#endif // !__DEBUGGING_H__