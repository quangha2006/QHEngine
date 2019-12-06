#include "GLDebugDrawer.h"
#include "Camera.h"
#include "Debugging.h"
#include <stdlib.h>

void GLDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

GLDebugDrawer::GLDebugDrawer()
	: mCurrentSizeBuffer(12)
	, m_debugMode(0)
	, mCurrentSize(0)
	, mSizeAllocated(0)
	, mfVertexData(NULL)
{
	const char* linesVertexShader =
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"uniform mat4 ProjectionMatrix;\n"
		"out lowp vec3 colourV;\n"
		"void main (void)\n"
		"{\n"
		"    colourV = aColor;\n"
		"		gl_Position = ProjectionMatrix * vec4(aPos, 1.0f);\n"
		"		\n"
		"}\n";
	const char* linesFragmentShader =
		"in lowp vec3 colourV;\n"
		"out lowp vec4 fragColour;\n"
		"void main(void)\n"
		"{\n"
		"    fragColour = vec4(colourV, 1.0f);\n"
		"}\n";

	linesShader.LoadShader(linesVertexShader, linesFragmentShader, true);

	lines_ProjectionMatrix = glGetUniformLocation(linesShader.program, "ProjectionMatrix");

	mSizeAllocated = 100;
	mfVertexData = (float*)std::malloc(mSizeAllocated * 6 * sizeof(float));
	mCurrentSizeBuffer = mSizeAllocated;

	glBindVertexArray(0);
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mSizeAllocated * 6, NULL, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float)));

	glBindVertexArray(0);


}

void GLDebugDrawer::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color)
{
	if (m_debugMode > 0)
	{
		if ((mCurrentSize + 2) >= mSizeAllocated)
		{
			float * more_vertexData = (float*)std::realloc(mfVertexData, (mSizeAllocated + 100) * 6 * sizeof(float));
			if (more_vertexData != NULL)
			{
				mfVertexData = more_vertexData;
				mSizeAllocated += 100;
			}
			else
			{
				LOGE("Error(re)allocating memory\n");
				return;
			}
		}
		mfVertexData[mCurrentSize * 6] = from.getX();
		mfVertexData[mCurrentSize * 6 + 1] = from.getY();
		mfVertexData[mCurrentSize * 6 + 2] = from.getZ();
		mfVertexData[mCurrentSize * 6 + 3] = color.getX();
		mfVertexData[mCurrentSize * 6 + 4] = color.getY();
		mfVertexData[mCurrentSize * 6 + 5] = color.getZ();
		++mCurrentSize;
		mfVertexData[mCurrentSize * 6] = to.getX();
		mfVertexData[mCurrentSize * 6 + 1] = to.getY();
		mfVertexData[mCurrentSize * 6 + 2] = to.getZ();
		mfVertexData[mCurrentSize * 6 + 3] = color.getX();
		mfVertexData[mCurrentSize * 6 + 4] = color.getY();
		mfVertexData[mCurrentSize * 6 + 5] = color.getZ();
		++mCurrentSize;
	}
}

void GLDebugDrawer::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & fromColor, const btVector3 & toColor)
{
	if (m_debugMode > 0)
	{
		if ((mCurrentSize + 2) >= mSizeAllocated)
		{
			float* more_vertexData = (float*)std::realloc(mfVertexData, (mSizeAllocated + 100) * 6 * sizeof(float));
			if (more_vertexData != NULL)
			{
				mfVertexData = more_vertexData;
				mSizeAllocated += 100;
			}
			else
			{
				LOGE("Error(re)allocating memory\n");
				return;
			}
		}
		mfVertexData[mCurrentSize * 6] = from.getX();
		mfVertexData[mCurrentSize * 6 + 1] = from.getY();
		mfVertexData[mCurrentSize * 6 + 2] = from.getZ();
		mfVertexData[mCurrentSize * 6 + 3] = fromColor.getX();
		mfVertexData[mCurrentSize * 6 + 4] = fromColor.getY();
		mfVertexData[mCurrentSize * 6 + 5] = fromColor.getZ();
		++mCurrentSize;
		mfVertexData[mCurrentSize * 6] = to.getX();
		mfVertexData[mCurrentSize * 6 + 1] = to.getY();
		mfVertexData[mCurrentSize * 6 + 2] = to.getZ();
		mfVertexData[mCurrentSize * 6 + 3] = toColor.getX();
		mfVertexData[mCurrentSize * 6 + 4] = toColor.getY();
		mfVertexData[mCurrentSize * 6 + 5] = toColor.getZ();
		++mCurrentSize;
	}
}

void GLDebugDrawer::flushLines()
{
//	glDisable(GL_DEPTH_TEST);
	if (m_debugMode > 0 && mCurrentSize > 0)
	{
		linesShader.use();
		
		glBindVertexArray(mVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glm::mat4 projectionMatrix = Camera::getInstance()->GetWorldViewProjectionMatrix();
		glm::mat4 modelViewMatrix = glm::mat4();
		glUniformMatrix4fv(lines_ProjectionMatrix, 1, false, &projectionMatrix[0][0]);

		glLineWidth(1);
		if (mSizeAllocated > mCurrentSizeBuffer)
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mSizeAllocated * 6, NULL, GL_DYNAMIC_DRAW);
			mCurrentSizeBuffer = mSizeAllocated;
		}
		//bind data
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * mCurrentSize * 6, mfVertexData);

		QHEngine::DrawArrays(GL_LINES, 0, mCurrentSize);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		CheckGLError("GLDebugDrawer::drawLine");
	}
	mCurrentSize = 0;
}


void GLDebugDrawer::drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color)
{
	drawLine(PointOnB, PointOnB + normalOnB * distance, color);
	btVector3 ncolor(0, 0, 0);
	drawLine(PointOnB, PointOnB + normalOnB * 0.01f, ncolor);
}

void GLDebugDrawer::reportErrorWarning(const char * warningString)
{
	LOGW("PhysicsSimulation reportErrorWarning: %s\n", warningString);
}

void GLDebugDrawer::draw3dText(const btVector3 & location, const char * textString)
{
	LOGI("location: %f, %f, %f String: %s\n", location.getX(), location.getY(), location.getZ(), textString);
}


GLDebugDrawer::~GLDebugDrawer()
{
	std::free(mfVertexData);
}
