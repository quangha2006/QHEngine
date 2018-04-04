#pragma once
#include "Shader.h"
class SkyBox
{
private:
	Shader mShader;
public:
	void Init(const char * texturepath);
	SkyBox();
	~SkyBox();
};

