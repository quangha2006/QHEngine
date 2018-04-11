#pragma once
#include "Shader.h"
#include <vector>

class SkyBox
{
private:
	Shader mShader;
	unsigned int textureID;
	unsigned int cubemapTexture;
public:
	void Init(const char * texturepath);
	unsigned int loadCubemap(std::vector<std::string> faces);
	SkyBox();
	~SkyBox();
};

