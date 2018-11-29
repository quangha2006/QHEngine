#include "Game.h"

int kayy = 0;
void Game::Init()
{
	mCamera->Pos = glm::vec3(-10.0f, 15.0f, 30.0f);
	mCamera->Target = glm::vec3(0.0f, 5.0f, 0.2f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);

	ShaderManager::getInstance()->Init("model", "Shaders/model_loading.vs", "Shaders/model_loading.fs");

	map1.Init("bountyhunter/bountyhunter/export_from_max/test.FBX", mCamera, true);
	map1.SetNeedRotate(false);
	map1.SetScale(glm::vec3(0.02f));

	sanctuary_arena_a_01.Init("sanctuary/sanctuary_arena_a_01.dae", mCamera, false);
	sanctuary_arena_a_01.SetNeedRotate(false);
	sanctuary_arena_a_01.SetTranslate(glm::vec3(8.0f, -9.75f, 0.0f));
	sanctuary_arena_a_01.SetRotate(180, glm::vec3(0.0f, 1.0f, 0.0f));
	sanctuary_arena_a_01.SetScale(glm::vec3(0.7f));

	//sanctuary_arena_b_01.Init("sanctuary/sanctuary_arena_b_01.dae", mCamera, false);
	//sanctuary_arena_b_01.SetNeedRotate(false);
	//sanctuary_arena_b_01.SetTranslate(glm::vec3(18.0f, 0.0f, -3.0f));
	//sanctuary_arena_b_01.SetRotate(220, glm::vec3(0.0f, 1.0f, 0.0f));
	//sanctuary_arena_b_01.SetScale(glm::vec3(0.7f));

	sanctuary_arena_tower_01.Init("sanctuary/sanctuary_stone_a_01.dae", mCamera, false);
	sanctuary_arena_tower_01.SetNeedRotate(false);
	sanctuary_arena_tower_01.SetTranslate(glm::vec3(3.0f, 0.0f, 0.0f));

	sanctuary_bird_01.Init("sanctuary/sanctuary_bird_animated_01.dae", mCamera, false);
	sanctuary_bird_01.SetNeedRotate(false);
	sanctuary_bird_01.SetTranslate(glm::vec3(4.15f, 5.725f, 0.0f));



}

void Game::Draw()
{
	map1.UpdateSkeleton();
	sanctuary_bird_01.UpdateSkeleton();
	ShaderManager::getInstance()->setUseProgram("model");
	glm::vec3 lampPos = glm::vec3(8.2f, 10.0f, 0.0f);
	map1.Draw(lampPos, 0);
	sanctuary_arena_tower_01.Draw(lampPos);
	sanctuary_bird_01.Draw(lampPos);
	sanctuary_arena_a_01.Draw(lampPos, kayy);
	sanctuary_arena_b_01.Draw(lampPos);
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
		kayy++;
		LOGI("%d\n", kayy);
		return;
	case 320: //num 0
		kayy--;
		LOGI("%d\n", kayy);
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