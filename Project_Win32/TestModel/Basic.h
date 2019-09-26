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
	Model m_Streetenvironment, ModelTest;

	QHAxis axis;
	bool m_initialized;
	Sprite *mbtSwitchShadow, *mbtSwitchBloom, *mbtBloomAmount1, *mbtBloomAmount2;
	QHText mShadowLabel, mBloomLabel, mBloomAmountLabel;
	glm::vec2 mDesignResolutionSize;
public:
	void Update(int delta);
	void Init();
	void Init2D();
	void GetRequireScreenSize(int32_t &width, int32_t &height);
	bool OnGameKeyPressed(int key, int scancode, int action, int mods);
	bool OnGameTouchEvent(int eventId, int x, int y, int pointerId);
	void OnGameLoadingThreadFinished(int loadingtimeinms);
	void ClickbuttonShadow();
	void ClickbuttonBloom();
	void ClickbuttonBloomAmountLeft();
	void ClickbuttonBloomAmountRight();
	Basic();
	~Basic();
};