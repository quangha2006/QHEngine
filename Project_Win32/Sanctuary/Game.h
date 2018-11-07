#pragma once
#include <AppBase.h>
#include <Shader.h>
#include <Model.h>
#include <Camera.h>
#include <ShaderManager.h>
#include <FrameBuffer.h>
#include "SkyBox.h"

class Game : public AppBase
{
private:
	Model map1, sanctuary_arena_tower_01, sanctuary_bird_01, sanctuary_arena_a_01, sanctuary_arena_b_01;
public:
	void Draw();
	void Init();
	void GetRequireScreenSize(int32_t &width, int32_t &height);
	void OnGameKeyPressed(int key, int scancode, int action, int mods);
	Game();
	~Game();
};

