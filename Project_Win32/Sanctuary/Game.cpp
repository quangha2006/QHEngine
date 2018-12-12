#include "Game.h"
#include "Timer.h"
float a = 9.0;
float b = -0.8f;
void Game::Init()
{
	mCamera->Pos = glm::vec3(40.0f, 3.0f, 18.0f);
	mCamera->Target = glm::vec3(1.0f, 1.0f, 1.0f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);

	ShaderManager::getInstance()->Init("model", "Shaders/model_loading.vs", "Shaders/model_loading.fs");

	bountyhunter.Init("bountyhunter/bountyhunter/export_from_max/test.FBX", mCamera, true);
	bountyhunter.SetNeedRotate(false);
	bountyhunter.SetScale(glm::vec3(0.015f));

	sanctuary_arena_a_01.Init("sanctuary/sanctuary_arena_a_01.dae", mCamera, false);
	sanctuary_arena_a_01.SetNeedRotate(false);
	//sanctuary_arena_a_01.SetTranslate(glm::vec3(8.0f, -9.75f, 0.0f));
	//sanctuary_arena_a_01.SetRotate(180, glm::vec3(0.0f, 1.0f, 0.0f));
	//sanctuary_arena_a_01.SetScale(glm::vec3(0.7f));
	sanctuary_arena_a_01.SetisUsePointLight(false);

	sanctuary_arena_b_01.Init("sanctuary/sanctuary_arena_b_01.dae", mCamera, false);
	sanctuary_arena_b_01.SetNeedRotate(false);
	sanctuary_arena_b_01.SetTranslate(glm::vec3(18.0f, 0.0f, -3.0f));
	sanctuary_arena_b_01.SetRotate(220, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_b_01.SetScale(glm::vec3(0.7f));

	sanctuary_arena_tower_01.Init("sanctuary/sanctuary_stone_a_01.dae", mCamera, false);
	sanctuary_arena_tower_01.SetNeedRotate(false);
	sanctuary_arena_tower_01.SetTranslate(glm::vec3(3.0f, 0.0f, 0.0f));

	sanctuary_bird_01.Init("sanctuary/sanctuary_bird_animated_01.dae", mCamera, false);
	sanctuary_bird_01.SetNeedRotate(false);
	sanctuary_bird_01.SetTranslate(glm::vec3(4.15f, 5.725f, 0.0f));

	sanctuary_torch_a_01.Init("sanctuary/sanctuary_torch_a_01.dae", mCamera, false);
	sanctuary_torch_a_01.SetNeedRotate(false);

	sanctuary_waterfall_01.Init("sanctuary/sanctuary_waterfall_01.dae", mCamera, false);
	sanctuary_waterfall_01.SetNeedRotate(false);
	sanctuary_waterfall_01.SetScale(glm::vec3(3.0f));
}

void Game::Draw()
{
	bountyhunter.UpdateSkeleton();
	sanctuary_bird_01.UpdateSkeleton();

	ShaderManager::getInstance()->setUseProgram("model");
	glm::vec3 lampPos = glm::vec3(8.2f, 10.0f, 0.0f);

	bountyhunter.Draw(lampPos, 0, true, glm::vec3(1210.6f, 0.0, 606.5f), false, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_tower_01.Draw(lampPos, 0, true, glm::vec3(15.6592f, -2.15f, 2.1054f), true, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	
	sanctuary_bird_01.Draw(lampPos, 0, true, glm::vec3(23.09f, -3.6f, 4.67f));
	sanctuary_bird_01.Draw(lampPos, 0, true, glm::vec3(19.256f, -4.15f, 0.7717f));

	sanctuary_arena_a_01.Draw(lampPos, 0, true, glm::vec3(20.6006f, 2.6813f, 20.47f), true, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));		//sanctuary_arch_01
	sanctuary_arena_a_01.Draw(lampPos, 1, true, glm::vec3(21.4392f,-13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));	//sanctuary_arena_a_ground_a_01
	sanctuary_arena_a_01.Draw(lampPos, 2, true, glm::vec3(21.4392f, -13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 3, true, glm::vec3(21.4392f, -13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 4, true, glm::vec3(21.4392f, -13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 5, true, glm::vec3(21.4392f, -13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 6, true, glm::vec3(21.4392f, -13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 7, true, glm::vec3(21.4392f, -13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 8, true, glm::vec3(21.4392f, -13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 9, true, glm::vec3(21.4392f, -13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 10, true, glm::vec3(21.4392f, -13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 11, true, glm::vec3(21.4392f, -13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 12, true, glm::vec3(21.4392f, -13.9883f, 27.0087f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 13, true, glm::vec3(17.6593f, -0.5003f, 27.3055f), true, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));	//sanctuary_arena_a_trunk_01
	sanctuary_arena_a_01.Draw(lampPos, 14, true, glm::vec3(32.5118f, -11.4552f, 22.7177f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));	//sanctuary_arena_a_water010
	sanctuary_arena_a_01.Draw(lampPos, 15, true, glm::vec3(25.7765f, -0.076f, 44.7686f), true, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));	//sanctuary_bridge_arena_a_01
	sanctuary_arena_a_01.Draw(lampPos, 16, true, glm::vec3(29.1565f, 3.6478f, -1.7717f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));	//sanctuary_rock_part_01
	sanctuary_arena_a_01.Draw(lampPos, 17, true, glm::vec3(29.1565f, 3.6478f, -1.7717f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 18, true, glm::vec3(28.7564f, 3.6478f, -1.7717f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 19, true, glm::vec3(28.9564f, 3.6478f, -0.9719f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 20, true, glm::vec3(29.3565f, 3.6478f, -1.1717f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.Draw(lampPos, 21, true, glm::vec3(28.6564f, 3.6478f, -1.1717f), true, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	
	sanctuary_torch_a_01.Draw(lampPos, 0, true, glm::vec3(24.445f, 2.9169f, 17.5039f), true, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_torch_a_01.Draw(lampPos, 0, true, glm::vec3(15.6449, 2.0f, 19.9f), true, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_waterfall_01.Draw(lampPos, -1, true, glm::vec3(a, 1.0f, b), false, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Game::GetRequireScreenSize(int32_t & width, int32_t & height)
{
	width = 900;
	height = 540;
}

void Game::OnGameKeyPressed(int key, int scancode, int action, int mods)
{
	if (action == 0) return;
	switch (key)
	{
	case 83: //num s
		mCamera->Target.y -= 0.1;
		return;
	case 87: //num w
		mCamera->Target.y += 0.1;
		return;
	case 65: //num a
		mCamera->Target.x -= 0.1;
		return;
	case 68: //num d
		mCamera->Target.x += 0.1;
		return;
	case 321: //num 1
		a+=0.1;
		LOGI("a %f\n", a);
		return;
	case 320: //num 0
		a-=0.1;
		LOGI("a %f\n", a);
		return;
	case 322: //num 1
		b-=0.1;
		LOGI("b %f\n", b);
		return;
	case 325: //num 0
		b+=0.1;
		LOGI("b %f\n", b);
		return;


	default:
		break;
	}
}

Game::Game()
{
}


Game::~Game()
{
}

AppBase* AppFactory()
{
	return new Game();
}