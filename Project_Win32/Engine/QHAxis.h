#ifndef __QHAXIS_H__
#define __QHAXIS_H__

#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

class QHAxis
{
private:
	GLuint program, vertexShader, fragmentShader;
	GLuint vboId;
	float *vertices_axis;
	Camera *camera;
	glm::mat4 model;
	GLint position_Attribute;
	GLint color_Attribute;
	GLint WorldViewProjectionMatrix_Uniform;
	bool createProgram();
	GLuint createShader(GLenum shaderType, const char * src);
	bool m_initialized;
public:
	void Draw();
	bool Init(Camera *camera);
	QHAxis();
	~QHAxis();
};

#endif // !__QHAXIS_H__