#pragma once
#include "LinearMath\btIDebugDraw.h"
#include "stdafx.h"
#include "Shader.h"
#include <vector>

class GLDebugDrawer : public btIDebugDraw
{
private:
	Shader linesShader;
	GLuint mVAO, mVBO;
	GLint lines_ModelViewMatrix = 0;
	GLint lines_ProjectionMatrix = 0;
	GLint lines_position = 0;
	GLint lines_colour = 0;
	int m_debugMode;
	std::vector<float> mVertexData;
	GLuint mCurrentSizeBuffer;
public:

	virtual void   drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

	virtual void   drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

	virtual void   reportErrorWarning(const char* warningString);

	virtual void   draw3dText(const btVector3& location, const char* textString) {};

	virtual void flushLines();

	virtual void   setDebugMode(int debugMode);

	virtual int    getDebugMode() const { return m_debugMode; }
	GLDebugDrawer();
	~GLDebugDrawer();
};

