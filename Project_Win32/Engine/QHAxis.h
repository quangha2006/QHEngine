#ifndef __QHAXIS_H__
#define __QHAXIS_H__

#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Shader.h"

class QHAxis
{
private:
	GLuint mVBO_Id;
	float *vertices_axis;
	Camera *mCamera;
	glm::mat4 model;
	GLint  mWorldView_uniform;
	bool m_initialized;
	Shader mShader;
public:
	void Draw();
	bool Init(Camera *camera);
	QHAxis();
	~QHAxis();
};

#endif // !__QHAXIS_H__