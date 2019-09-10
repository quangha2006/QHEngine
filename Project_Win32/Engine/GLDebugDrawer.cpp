#include "GLDebugDrawer.h"
#include "Camera.h"

void GLDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

GLDebugDrawer::GLDebugDrawer()
{
	const char* linesVertexShader =
		"uniform mat4 ModelViewMatrix;\n"
		"uniform mat4 ProjectionMatrix;\n"
		"uniform vec4 colour;\n"
		"in vec4 position;\n"
		"out vec4 colourV;\n"
		"void main (void)\n"
		"{\n"
		"    colourV = colour;\n"
		"		gl_Position = ProjectionMatrix * ModelViewMatrix * position;\n"
		"		\n"
		"}\n";
	const char* linesFragmentShader =
		"in vec4 colourV;\n"
		"out vec4 fragColour;\n"
		"void main(void)\n"
		"{\n"
		"    fragColour = colourV;\n"
		"}\n";

	linesShader.LoadShader(linesVertexShader, linesFragmentShader, true);
	lines_ModelViewMatrix = glGetUniformLocation(linesShader.program, "ModelViewMatrix");
	lines_ProjectionMatrix = glGetUniformLocation(linesShader.program, "ProjectionMatrix");
	lines_colour = glGetUniformLocation(linesShader.program, "colour");
	lines_position = glGetAttribLocation(linesShader.program, "position");
}

void GLDebugDrawer::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color)
{
	if (m_debugMode > 0)
	{
		linesShader.use();
		glm::mat4 projectionMatrix = Camera::getInstance()->GetWorldViewProjectionMatrix();
		glm::mat4 modelViewMatrix = glm::mat4();
		glUniformMatrix4fv(lines_ProjectionMatrix, 1, false, &projectionMatrix[0][0]);
		glUniformMatrix4fv(lines_ModelViewMatrix, 1, false, &modelViewMatrix[0][0]);
		glUniform4f(lines_colour, color.getX(), color.getY(), color.getZ(), 1.0f);

		glLineWidth(1);
		glBegin(GL_LINES);
		glColor3f(color.getX(), color.getY(), color.getZ());
		glVertex3d(from.getX(), from.getY(), from.getZ());
		glColor3f(color.getX(), color.getY(), color.getZ());
		glVertex3d(to.getX(), to.getY(), to.getZ());
		glEnd();
	}
}

void GLDebugDrawer::drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color)
{
	drawLine(PointOnB, PointOnB + normalOnB * distance, color);
	btVector3 ncolor(0, 0, 0);
	drawLine(PointOnB, PointOnB + normalOnB * 0.01, ncolor);
}


GLDebugDrawer::~GLDebugDrawer()
{
}
