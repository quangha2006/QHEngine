#include "Sprite.h"

bool Sprite::isTouchOnSprite(int x, int y)
{
	if (x >= mPos.x && x <= mPos.x + mTexture.width && y >= mPos.y && y <= mPos.y + mTexture.height)
		return true;
	return false;
}

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

void Sprite::SetCallbackOnTouchBegan(void (*callback)())
{
	mCallbackTouchBegan = callback;
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

bool Sprite::OnTouchEvent(int eventId, int x, int y, int pointerId)
{
	if (isTouchOnSprite(x,y)) //check is touch on sprite
	{
		if (mCallbackTouchBegan !=nullptr && eventId == 0)
			(*mCallbackTouchBegan)();

		return true;
	}
	return false;
}

Sprite::Sprite()
{
	mCallbackTouchBegan = nullptr;
}


Sprite::~Sprite()
{
}
