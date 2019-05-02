#pragma once
#include "Shader.h"

class UserInterface
{
private:
	Shader mShader;
public:
	bool Init();
	UserInterface();
	~UserInterface();
};

