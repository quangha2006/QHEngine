#include "GLDebugDrawer.h"

void GLDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

GLDebugDrawer::GLDebugDrawer()
{
}

void GLDebugDrawer::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color)
{
	//      if (m_debugMode > 0)
	{
		float tmp[6] = { from.getX(), from.getY(), from.getZ(),
					  to.getX(), to.getY(), to.getZ() };

		glPushMatrix();
		{
			glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);
			glVertexPointer(3,
				GL_FLOAT,
				0,
				&tmp);

			glPointSize(5.0f);
			glDrawArrays(GL_POINTS, 0, 2);
			glDrawArrays(GL_LINES, 0, 2);
		}
		glPopMatrix();
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
