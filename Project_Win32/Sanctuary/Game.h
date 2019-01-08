#pragma once
#include <AppBase.h>
#include <Shader.h>
#include <Model.h>
#include <Camera.h>
#include <ShaderManager.h>
#include <FrameBuffer.h>
#include "SkyBox.h"
#include <QHAxis.h>

class Game : public AppBase
{
private:
	Model bountyhunter,
		sanctuary_arena_tower_01,
		sanctuary_bird_01,
		sanctuary_arena_a_01,
		sanctuary_arena_b_01,
		sanctuary_torch_a_01,
		sanctuary_waterfall_01;
	FrameBuffer testHDR;
	QHAxis axis;
	bool m_initialized;
public:
	void Draw();
	void Init();
	void LoadingThread(ShareContext *shared_context);
	void GetRequireScreenSize(int32_t &width, int32_t &height);
	void OnGameKeyPressed(int key, int scancode, int action, int mods);
	Game();
	~Game();
};

