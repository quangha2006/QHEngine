#include "QHAxis.h"
#include "stdafx.h"

bool QHAxis::Init(Camera *camera)
{
	char vtxSrc[] = {
		"#version 100\n"
		"attribute vec3 aPos;\n"
		"attribute vec3 aColor;\n"
		"\n"
		"varying vec3 thecolor;\n"
		"uniform mat4 WorldViewProjectionMatrix;\n"
		"void main()\n"
		"{\n"
		"	thecolor = aColor;\n"
		"	gl_Position =  WorldViewProjectionMatrix * vec4(aPos, 1.0);\n"
		"}\n"
	};
	char fragSrc[] = {
		"precision highp float;\n"
		"\n"
		"varying vec3 thecolor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = vec4(thecolor, 1.0);\n"
		"}\n"
	};
	
	if (mShader.LoadShader(vtxSrc, fragSrc, true))
	{
		mWorldView_uniform = glGetUniformLocation(mShader.program, "WorldViewProjectionMatrix");

		glGenBuffers(1, &mVBO_Id);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO_Id);
		glBufferData(GL_ARRAY_BUFFER, 15 * 6 * sizeof(float), vertices_axis, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		mCamera = camera;
		m_initialized = true;
		return true;
	}
	return false;
}

void QHAxis::Draw()
{
	if (!m_initialized) return;

	glEnable(GL_DEPTH_TEST);

	mShader.use();

	GLint position_attribute = mShader.getPosAttribute();
	GLint color_attribute = mShader.getColorAttribute();

	glBindBuffer(GL_ARRAY_BUFFER, mVBO_Id);

	if (position_attribute != -1)
	{
		glEnableVertexAttribArray(position_attribute);
		glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	}

	if (color_attribute != -1)
	{
		glEnableVertexAttribArray(color_attribute);
		glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	if (mWorldView_uniform != -1)
	{
		glm::mat4 lookat_tmp = mCamera->GetWorldViewProjectionMatrix() * model;

		glUniformMatrix4fv(mWorldView_uniform, 1, GL_FALSE, &lookat_tmp[0][0]);
	}

	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_TRIANGLES, 2, 3);

	glDrawArrays(GL_LINES, 5, 2);
	glDrawArrays(GL_TRIANGLES, 7, 3);

	glDrawArrays(GL_LINES, 10, 2);
	glDrawArrays(GL_TRIANGLES, 12, 3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisable(GL_DEPTH_TEST);
}

QHAxis::QHAxis()
	: m_initialized(false)
	, model(glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)))
	, mWorldView_uniform(-1)
	, mVBO_Id(0)
	, mCamera(nullptr)
{
	vertices_axis = new float[15 * 6]{
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
		0.3f, 0.0f,  10.0f, 0.0f, 0.0f, 1.0f,
		-0.3f, 0.0f,  10.0f, 0.0f, 0.0f, 1.0f
	};
}

QHAxis::~QHAxis()
{
	delete[] vertices_axis;
	glDeleteBuffers(1, &mVBO_Id);
}
