#pragma once
#include "Shader.h"
#include "Camera.h"
#include <vector>

class SkyBox
{
private:
	Shader mShader;
	//unsigned int textureID;
	unsigned int cubemapTexture;
	float *skyboxVertices;
	unsigned int VBO;
public:
	void Init(const char * texturepath);
	void Draw(Camera *camera);
	unsigned int loadCubemap(const char * texturepath, std::vector<std::string> faces);
	SkyBox();
	~SkyBox();
};