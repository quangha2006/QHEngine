#pragma once
#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "QHTexture.h"
#include "Utils.h"

class Sprite
{
private:
	Texture mTexture;
	glm::vec2 mPos;
	void (*mCallbackTouchBegan)();
	bool isTouchOnSprite(int x, int y);
public:
	GLuint GetTexId();
	glm::vec2 GetPos();
	int getWidth();
	int getHeight();
	void SetCallbackOnTouchBegan(void(*callback)());
	bool LoadTexture(const char *path);
	void SetPos(int x, int y);
	bool OnTouchEvent(int eventId, int x, int y, int pointerId);
	Sprite();
	~Sprite();
};