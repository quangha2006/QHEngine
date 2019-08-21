#pragma once
#include <AppBase.h>
#include <AppContext.h>
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "SkyBox.h"
#include "Sound.h"
#include "QHAxis.h"
#include "UserInterface.h"
#include "QHText.h"

class GameDemo : public AppBase
{
private:
	Model m_Streetenvironment, mSpider;
public:
	void Update(int delta);
	void Init();
	void GetRequireScreenSize(int32_t &width, int32_t &height);
	bool OnGameKeyPressed(int key, int scancode, int action, int mods);
	bool OnGameTouchEvent(int eventId, int x, int y, int pointerId);
	void OnGameLoadingThreadFinished(int loadingtimeinms);
	GameDemo();
	~GameDemo();
};