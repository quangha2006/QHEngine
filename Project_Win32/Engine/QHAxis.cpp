#include "QHAxis.h"
#include "stdafx.h"

bool QHAxis::Init(Camera *camera)
{
	if (createProgram())
	{
		mPosition_attribute = glGetAttribLocation(mProgram, "aPos");
		mColor_attribute = glGetAttribLocation(mProgram, "aColor");
		mWorldView_uniform = glGetUniformLocation(mProgram, "WorldViewProjectionMatrix");

		glGenBuffers(1, &mVBO_Id);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO_Id);
		glBufferData(GL_ARRAY_BUFFER, 15 * 6 * sizeof(float), vertices_axis, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		mCamera = camera;
		m_initialized = true;
	}
	return true;
}

void QHAxis::Draw()
{
	if (!m_initialized) return;

	glEnable(GL_DEPTH_TEST);
	glUseProgram(mProgram);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO_Id);

	if (mPosition_attribute != -1)
	{
		glEnableVertexAttribArray(mPosition_attribute);
		glVertexAttribPointer(mPosition_attribute, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	}

	if (mColor_attribute != -1)
	{
		glEnableVertexAttribArray(mColor_attribute);
		glVertexAttribPointer(mColor_attribute, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	if (mWorldView_uniform != -1)
	{
		glm::mat4 lookat_tmp = mCamera->WorldViewProjectionMatrix * model;

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

bool QHAxis::createProgram()
{
	GLint linked = GL_FALSE;
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

	mVertexShader = createShader(GL_VERTEX_SHADER, vtxSrc);
	if (mVertexShader == -1)
	{
		return false;
	}
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
	mFragmentShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
	if (mFragmentShader == -1)
	{
		return false;
	}
	mProgram = glCreateProgram();
	if (mProgram == -1) {
		return false;
	}
	glAttachShader(mProgram, mVertexShader);
	glAttachShader(mProgram, mFragmentShader);

	glLinkProgram(mProgram);
	glGetProgramiv(mProgram, GL_LINK_STATUS, &linked);
	if (!linked) {
		LOGI("Could not link program\n");
		GLint infoLogLen = 0;
		glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen) {
			GLchar* infoLog = (GLchar*)malloc(infoLogLen);
			if (infoLog) {
				glGetProgramInfoLog(mProgram, infoLogLen, NULL, infoLog);
				LOGI("Could not link program:\n%s\n", infoLog);
				free(infoLog);
			}
		}
		glDeleteProgram(mProgram);
		mProgram = -1;
		return false;
	}
	return true;
}

GLint QHAxis::createShader(GLenum shaderType, const char *src)
{
	GLint shader = glCreateShader(shaderType);
	if (shader == -1) {
		return 0;
	}
	glShaderSource(shader, 1, &src, NULL);

	GLint compiled = GL_FALSE;

	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLogLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen > 0) {
			GLchar* infoLog = (GLchar*)malloc(infoLogLen);
			if (infoLog) {
				glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
				LOGI("Could not compile %s shader:\n%s\n", shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
				free(infoLog);
			}
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}


QHAxis::QHAxis()
	: m_initialized(false)
	, model(glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)))
	, mProgram(-1)
	, mVertexShader(-1)
	, mFragmentShader(-1)
	, mPosition_attribute(-1)
	, mColor_attribute(-1)
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
