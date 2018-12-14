#include "QHAxis.h"

#include "stdafx.h"
#include "QHAxis.h"


bool QHAxis::Init(Camera *camera)
{
	if (createProgram())
	{
		position_Attribute = glGetAttribLocation(program, "aPos");
		color_Attribute = glGetAttribLocation(program, "aColor");
		WorldViewProjectionMatrix_Uniform = glGetUniformLocation(program, "WorldViewProjectionMatrix");
	}
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, 15 * 6 * sizeof(float), vertices_axis, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	this->camera = camera;
	return true;
}

void QHAxis::Draw()
{
	if (program == 0) return;
	glUseProgram(program);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);

	if (position_Attribute != -1)
	{
		glEnableVertexAttribArray(position_Attribute);
		glVertexAttribPointer(position_Attribute, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	}

	if (color_Attribute != -1)
	{
		glEnableVertexAttribArray(color_Attribute);
		glVertexAttribPointer(color_Attribute, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glm::mat4 lookat_tmp = camera->WorldViewProjectionMatrix * model;

	glUniformMatrix4fv(WorldViewProjectionMatrix_Uniform, 1, GL_FALSE, &lookat_tmp[0][0]);

	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_TRIANGLES, 2, 3);

	glDrawArrays(GL_LINES, 5, 2);
	glDrawArrays(GL_TRIANGLES, 7, 3);

	glDrawArrays(GL_LINES, 10, 2);
	glDrawArrays(GL_TRIANGLES, 12, 3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	vertexShader = createShader(GL_VERTEX_SHADER, vtxSrc);
	if (!vertexShader)
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
	fragmentShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
	if (!fragmentShader)
	{
		return false;
	}
	program = glCreateProgram();
	if (!program) {
		return false;
	}
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		LOGI("Could not link program\n");
		GLint infoLogLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen) {
			GLchar* infoLog = (GLchar*)malloc(infoLogLen);
			if (infoLog) {
				glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
				LOGI("Could not link program:\n%s\n", infoLog);
				free(infoLog);
			}
		}
		glDeleteProgram(program);
		program = 0;
	}
	return true;
}

GLuint QHAxis::createShader(GLenum shaderType, const char *src)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) {
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
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
}

QHAxis::~QHAxis()
{
	delete[] vertices_axis;
	glDeleteBuffers(1, &vboId);
}
