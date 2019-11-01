#pragma once
#include "GameDemo.h"

void GameDemo::Update(int delta)
{
}

void GameDemo::Init()
{
	mCamera->SetPos(0.0f, 5.0f, 20.0f);
	mCamera->SetTarget(0.0f, 5.0f, 0.0f);
	mCamera->SetLightPos(20.2f, 50.0f, 1.0f);

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("Streetenvironment/Street environment_V01.obj", true, false);
	m_Streetenvironment.SetPos(glm::vec3(0.0f, -0.03f, 0.5f));
	m_Streetenvironment.SetIsDrawDepthMap(false);
	m_Streetenvironment.CreateBoxShapePhysicsBody(0.0f, glm::vec3(48.0, .1, 48.0), glm::vec3(0., 0.001, 0.));
	m_Streetenvironment.GetRigidBody()->setFriction(0.0);
	m_Streetenvironment.GetRigidBody()->setRestitution(1.);

	mSpider.Init("Low-Poly Spider/Spider_3.fbx", true);
	mSpider.SetRotate(180.0, glm::vec3(.0f, 1.0f, .0f));
	mSpider.SetScale(glm::vec3(0.05f));
	mSpider.SetPos(glm::vec3(-8.0f, 0.0f, 0.0f));
	mSpider.SetAnimPlay(1);
}

void GameDemo::GetRequireScreenSize(int32_t & width, int32_t & height)
{
	width = 960;
	height = 540;
}

bool GameDemo::OnGameKeyPressed(int key, int scancode, int action, int mods)
{
	//LOGI("key: %d\n", key);
	if (action != 0) return true;
	int anim;
	switch (key)
	{
	case 333:
		anim = mSpider.GetAnimPlay() + 1;
		mSpider.SetAnimPlay(anim);
		LOGI("mSpider playing anim: %d | %s\n", mSpider.GetAnimPlay(), mSpider.GetAnimNamePlaying());
		break;
	case 334:
		anim = mSpider.GetAnimPlay() - 1;
		mSpider.SetAnimPlay(anim);
		LOGI("mSpider playing anim: %d | %s\n", mSpider.GetAnimPlay(), mSpider.GetAnimNamePlaying());
		break;
	}
	return true;
}

bool GameDemo::OnGameTouchEvent(int eventId, int x, int y, int pointerId)
{
	return false;
}

void GameDemo::OnGameLoadingThreadFinished(int loadingtimeinms)
{
}

GameDemo::GameDemo()
{
}

GameDemo::~GameDemo()
{
}
AppBase* AppFactory()
{
	return new GameDemo();
}