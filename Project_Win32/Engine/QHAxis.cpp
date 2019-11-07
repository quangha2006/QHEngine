#include "QHAxis.h"
#include "stdafx.h"

bool QHAxis::Init(Camera *camera)
{
	char vtxSrc[] = {
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"\n"
		"out vec3 thecolor;\n"
		"uniform mat4 WorldViewProjectionMatrix;\n"
		"void main()\n"
		"{\n"
		"	thecolor = aColor;\n"
		"	gl_Position =  WorldViewProjectionMatrix * vec4(aPos, 1.0);\n"
		"}\n"
	};
	char fragSrc[] = {
		"precision highp float;\n"
		"layout(location = 0) out vec4 FragColor;\n"
		"\n"
		"in vec3 thecolor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(thecolor, 1.0);\n"
		"}\n"
	};
	
	if (mShader.LoadShader(vtxSrc, fragSrc, true))
	{
		mWorldView_uniform = glGetUniformLocation(mShader.program, "WorldViewProjectionMatrix");

		glGenBuffers(1, &mVBO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, 15 * 6 * sizeof(float), vertices_axis, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		mCamera = camera;
		m_initialized = true;
		return true;
	}
	return false;
}

void QHAxis::Render()
{
	if (!m_initialized) return;

	//glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	mShader.use();

	GLint position_attribute = mShader.getPosAttribute();
	GLint color_attribute = mShader.getColorAttribute();

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	if (mWorldView_uniform != -1)
	{
		glm::mat4 lookat_tmp = mCamera->GetWorldViewProjectionMatrix() * model;

		glUniformMatrix4fv(mWorldView_uniform, 1, GL_FALSE, &lookat_tmp[0][0]);
	}
	glLineWidth(2);
	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_TRIANGLES, 2, 3);

	glDrawArrays(GL_LINES, 5, 2);
	glDrawArrays(GL_TRIANGLES, 7, 3);

	glDrawArrays(GL_LINES, 10, 2);
	glDrawArrays(GL_TRIANGLES, 12, 3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glDisable(GL_DEPTH_TEST);
}

QHAxis::QHAxis()
	: m_initialized(false)
	, model(glm::mat4())
	, mWorldView_uniform(-1)
	, mVBO(0)
	, mVAO(0)
	, mCamera(nullptr)
{
	vertices_axis = new float[15 * 6]{
		// x_axis
		-10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		10.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		10.3f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		10.0f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
		10.0f, -0.3f, 0.0f, 1.0f, 0.0f, 0.0f,
		// y_axis
		0.0f, -10.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 10.3f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.3f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.3f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		// z_axis
		0.0f, 0.0f, -10.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f,  10.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f,  10.3f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.3f,  10.0f, 0.0f, 0.0f, 1.0f,
		0.0f, -0.3f,  10.0f, 0.0f, 0.0f, 1.0f
	};
}

QHAxis::~QHAxis()
{
	delete[] vertices_axis;
	glDeleteBuffers(1, &mVBO);
}
