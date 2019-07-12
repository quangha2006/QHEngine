#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "QHTexture.h"
#include "Utils.h"
#include <functional>

class Sprite
{
private:
	Texture mTexture;
	glm::vec2 mPos;
	float mScale, mScaleX, mScaleY;
	std::function<void()> mCallbackTouchBegan;
	bool isTouchOnSprite(int x, int y);
	std::string mLabel;
	float mAlpha;
	bool mIsGray;
public:
	GLuint GetTexId();
	glm::vec2 GetPos();
	float GetAlpha();
	int getWidth();
	int getHeight();
	/**
	example: SetCallbackOnTouchBegan(std::bind(&Basic::ClickbuttonShadow, this));
	**/
	void SetCallbackOnTouchBegan(std::function<void()> callback);
	bool LoadTexture(const char *path);
	void SetPos(int x, int y);
	void SetScale(float scale);
	void SetGrayOut(bool isgrayout);
	bool IsGrayOut();
	void SetAlpha(float alpha);
	bool OnTouchEvent(int eventId, int x, int y, int pointerId);
	Sprite();
	~Sprite();
};

#endif //!__SPRITE_H__