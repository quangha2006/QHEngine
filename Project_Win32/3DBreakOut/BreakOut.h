#pragma once
#include <AppBase.h>
#include <AppContext.h>
#include <SkyBox.h>
#include <Model.h>

class BreakOut : public AppBase
{
private:
	Model m_Streetenvironment, cube, uvcircle;
	Model top, botton, left, right;
	Model listcube[5*5*5];
public:
	void Update();
	void Init();
	void GetRequireScreenSize(int32_t &width, int32_t &height);
	void OnGameKeyPressed(int key, int scancode, int action, int mods);
	BreakOut();
	~BreakOut();
};

