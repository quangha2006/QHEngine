#include "Sprite.h"

GLuint Sprite::GetTexId()
{
	return mTexture.id;
}

glm::vec2 Sprite::GetPos()
{
	return mPos;
}

int Sprite::getWidth()
{
	return mTexture.width;
}

int Sprite::getHeight()
{
	return mTexture.height;
}

bool Sprite::LoadTexture(const char * path)
{
	mTexture.id = QHTexture::TextureFromFile(path, Utils::getResourcesFolder(), mTexture.width, mTexture.height);
	if (mTexture.id == -1)
		return false;
	return true;
}

void Sprite::SetPos(int x, int y)
{
	mPos = glm::vec2(x, y);
}

Sprite::Sprite()
{
}


Sprite::~Sprite()
{
}
