#pragma once
#include <AppBase.h>
#include <AppContext.h>
#include <SkyBox.h>
#include <Model.h>
#include <Sound.h>

class BreakOut : public AppBase
{
private:
	Model m_Streetenvironment, cube, uvcircle;
	Model top, botton, left, right;
	Sound mSoundCollistion;
	Model listcube[4 * 5];
public:
	void Update();
	void Init();
	void GetRequireScreenSize(int32_t &width, int32_t &height);
	bool OnGameKeyPressed(int key, int scancode, int action, int mods);
	bool OnGameTouchEvent(int eventId, int x, int y, int pointerId);
	bool OnGameZoomCamera(double xoffset, double yoffset);
	void GameReset();
	BreakOut();
	~BreakOut();
};

