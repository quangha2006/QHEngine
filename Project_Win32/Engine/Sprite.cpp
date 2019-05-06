#include "Sprite.h"



GLuint Sprite::GetTexId()
{
	return m_TextureID;
}

glm::vec2 Sprite::GetPos()
{
	return mPos;
}

Sprite::Sprite()
{
}


Sprite::~Sprite()
{
}
