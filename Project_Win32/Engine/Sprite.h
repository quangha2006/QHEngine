#pragma once
#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Sprite
{
private:
	GLuint m_TextureID;
	glm::vec2 mPos;
public:
	Sprite();
	~Sprite();
};

