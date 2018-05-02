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
	GLuint *skyboxIndices;
	GLuint VBO;
	GLuint EBO;
	float scale;
	glm::mat4 model;
public:
	void Init(const char * texturepath);
	void Draw(Camera *camera);
	void setScale(float scale);
	unsigned int loadCubemap(const char * texturepath, std::vector<std::string> faces);
	SkyBox();
	~SkyBox();
};