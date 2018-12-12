#pragma once
#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class QHAxis
{
private:
	GLuint program, vertexShader, fragmentShader;
	GLuint vboId;
	float *vertices_axis;
	glm::mat4 model;
	GLint position_Attribute;
	GLint color_Attribute;
	GLint model_Uniform;
	GLint view_Uniform;
	GLint projection_Uniform;
	bool createProgram();
	GLuint createShader(GLenum shaderType, const char * src);
public:
	void Draw(glm::mat4 view, glm::mat4 projection);
	bool Init();
	QHAxis();
	~QHAxis();
};

