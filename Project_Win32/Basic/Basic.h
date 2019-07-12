#pragma once
#include <AppBase.h>
#include <AppContext.h>
#include <Shader.h>
#include <Model.h>
#include <Camera.h>
#include <ShaderManager.h>
#include "SkyBox.h"
#include "Sound.h"
#include "QHAxis.h"
#include "UserInterface.h"
#include "QHText.h"


class Basic : public AppBase
{
private:
	Model mNanosuit, m_Streetenvironment, mMerce, mSpider, saberclass, mGallacticCruiser, mMonster_1, uvcircle;
	float timestamp_for_lamp;
	Sound soundIntro;
	QHAxis axis;
	bool m_initialized;
	//Sprite *mbutton;
	Sprite *mbtSwitchShadow, *mbtSwitchBloom;
	QHText mShadowLabel, mBloomLabel;
	glm::vec2 mDesignResolutionSize;
public:
	void Update(int delta);
	void Init();
	void GetRequireScreenSize(int32_t &width, int32_t &height);
	bool OnGameKeyPressed(int key, int scancode, int action, int mods);
	bool OnGameTouchEvent(int eventId, int x, int y, int pointerId);
	void OnGameLoadingThreadFinished(int loadingtimeinms);
	void ClickbuttonShadow();
	void ClickbuttonBloom();
	Basic();
	~Basic();
};