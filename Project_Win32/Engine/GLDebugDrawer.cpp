#include "GLDebugDrawer.h"
#include "Camera.h"
#include "Debugging.h"


void GLDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

GLDebugDrawer::GLDebugDrawer()
	: mCurrentSizeBuffer(12)
{
	const char* linesVertexShader =
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"uniform mat4 ProjectionMatrix;\n"
		"out vec3 colourV;\n"
		"void main (void)\n"
		"{\n"
		"    colourV = aColor;\n"
		"		gl_Position = ProjectionMatrix * vec4(aPos, 1.0f);\n"
		"		\n"
		"}\n";
	const char* linesFragmentShader =
		"in vec3 colourV;\n"
		"out vec4 fragColour;\n"
		"void main(void)\n"
		"{\n"
		"    fragColour = vec4(colourV, 1.0f);\n"
		"}\n";

	linesShader.LoadShader(linesVertexShader, linesFragmentShader, true);

	lines_ProjectionMatrix = glGetUniformLocation(linesShader.program, "ProjectionMatrix");

	glBindVertexArray(0);
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mCurrentSizeBuffer, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(linesShader.position_Attribute);
	glVertexAttribPointer(linesShader.position_Attribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);

	glEnableVertexAttribArray(linesShader.color_Attribute);
	glVertexAttribPointer(linesShader.color_Attribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
}

void GLDebugDrawer::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color)
{
	std::vector<float> tmpdata{ from.getX(),	from.getY(),	from.getZ(),	color.getX(),	color.getY(),	color.getZ(),
						to.getX(),		to.getY(),		to.getZ() ,		color.getX(),	color.getY(),	color.getZ() };

	mVertexData.insert(mVertexData.end(), tmpdata.begin(), tmpdata.end());
}

void GLDebugDrawer::flushLines()
{
	if (m_debugMode > 0)
	{
		linesShader.use();
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBindVertexArray(mVAO);

		glm::mat4 projectionMatrix = Camera::getInstance()->GetWorldViewProjectionMatrix();
		glm::mat4 modelViewMatrix = glm::mat4();
		glUniformMatrix4fv(lines_ProjectionMatrix, 1, false, &projectionMatrix[0][0]);

		glLineWidth(1);
		if (mVertexData.size() > mCurrentSizeBuffer)
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mVertexData.size(), NULL, GL_DYNAMIC_DRAW);
			mCurrentSizeBuffer = mVertexData.size();
		}
		//bind data
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * mVertexData.size(), &mVertexData[0]);

		QHEngine::DrawArrays(GL_LINES, 0, mVertexData.size() / 6);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		CheckGLError("GLDebugDrawer::drawLine");
	}
	mVertexData.clear();
}


void GLDebugDrawer::drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color)
{
	drawLine(PointOnB, PointOnB + normalOnB * distance, color);
	btVector3 ncolor(0, 0, 0);
	drawLine(PointOnB, PointOnB + normalOnB * 0.01, ncolor);
}

void GLDebugDrawer::reportErrorWarning(const char * warningString)
{
	LOGW("PhysicsSimulation reportErrorWarning: %s\n", warningString);
}


GLDebugDrawer::~GLDebugDrawer()
{
}
