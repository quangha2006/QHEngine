#pragma once
#include "Shader.h"
#include "Camera.h"
#include <vector>

class SkyBox
{
private:
	Shader mShader;
	GLuint textureID;
	GLfloat *skyboxVertices;
	GLuint VBO;
	float scale;
	glm::mat4 model;

	unsigned int loadCubemap(const char * texturepath, std::vector<std::string> faces);
	GLuint getTextureID();
	bool m_initialized;
public:
	void Init(const char * texturepath);
	void Draw(Camera *camera);
	void setScale(float scale);
	SkyBox();
	~SkyBox();
};