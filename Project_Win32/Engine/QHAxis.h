#ifndef __QHAXIS_H__
#define __QHAXIS_H__

#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

class QHAxis
{
private:
	GLint mProgram, mVertexShader, mFragmentShader;
	GLuint mVBO_Id;
	float *vertices_axis;
	Camera *mCamera;
	glm::mat4 model;
	GLint mPosition_attribute, mColor_attribute, mWorldView_uniform;
	bool m_initialized;

	bool createProgram();
	GLint createShader(GLenum shaderType, const char * src);
public:
	void Draw();
	bool Init(Camera *camera);
	QHAxis();
	~QHAxis();
};

#endif // !__QHAXIS_H__