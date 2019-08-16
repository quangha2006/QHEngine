#pragma once
#include "GameDemo.h"
#include <iostream>

void GameDemo::Update(int delta)
{
}

void GameDemo::Init()
{
	mCamera->Pos = glm::vec3(5.0f, 5.0f, 20.0f);
	mCamera->Target = glm::vec3(0.0f, 1.0f, 0.0f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	mCamera->lightPos = glm::vec3(8.2f, 10.0f, 9.0f);

	mSkyBox.Init("SkyBox");
}

void GameDemo::GetRequireScreenSize(int32_t & width, int32_t & height)
{
}

bool GameDemo::OnGameKeyPressed(int key, int scancode, int action, int mods)
{
	return false;
}

bool GameDemo::OnGameTouchEvent(int eventId, int x, int y, int pointerId)
{
	return false;
}

void GameDemo::OnGameLoadingThreadFinished(int loadingtimeinms)
{
}
