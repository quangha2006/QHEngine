#pragma once
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Sprite.h"
#include <vector>

class UserInterface
{
private:
	static UserInterface* instance;
	Shader mShader;
	glm::mat4 mProjection;
	std::vector<Sprite*> mListUI;
public:
	static UserInterface* getInstance();
	bool Init(int width, int height);
	void Render();
	UserInterface();
	~UserInterface();
};