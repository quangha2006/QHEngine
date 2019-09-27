#pragma once
#include "Basic.h"
#include "Logs.h"
#include "TextRendering.h"
#include "FrameRate.h"
#include "Timer.h"
#include "Debugging.h"
#include "QHText.h"
#include "SkyBox.h"
#include "Utils.h"
#include "RenderManager.h"
#include "PhysicsSimulation.h"

const int Screen_width = 1280;
const int Screen_height = 720;

void Basic::Init()
{
	mCamera->SetPos(0.0f, 10.0f, 10.0f);
	mCamera->SetTarget(0.0f, 5.0f, 0.0f);
	mCamera->SetLightPos(20.2f, 20.0f, 1.0f);

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("GameDemo/PCcrasher.fbx", true, true);
	m_Streetenvironment.SetPos(glm::vec3(0.0f, 0.5f, 0.0f));
	//m_Streetenvironment.SetScale(glm::vec3(100.0f,50.0f,100.0f));
	m_Streetenvironment.SetIsDrawDepthMap(true);
	//m_Streetenvironment.SetDrawWireFrame(true);
	//m_Streetenvironment.SetRenderMode(RenderMode::RenderMode_Mesh);
	m_Streetenvironment.CreateBoxShapePhysicsBody(0.0f, glm::vec3(48.0, 1., 48.0), glm::vec3(0., -0.48, 0.));
	//m_Streetenvironment.GetRigidBody()->setFriction(0.0);
	//m_Streetenvironment.GetRigidBody()->setRestitution(1.);

	//ModelTest.Init("bountyhunter/bountyhunter/export_from_max/bountyhunter_run_01.FBX", true);
	ModelTest.Init("bountyhunter/bountyhunter/export_from_max/test.FBX", true);
	ModelTest.SetPos(glm::vec3(0.f, 10.0f, 0.0f));
	ModelTest.SetScale(glm::vec3(0.02f));
	ModelTest.SetDrawMesh(3);
	ModelTest.SetDrawWireFrame(false);
	ModelTest.CreateBoxShapePhysicsBody(1.0, glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, -3.0f, 0.0f));

	axis.Init(mCamera);
	m_initialized = true;
	RenderManager::getInstance()->SetEnableBloom(true);
	Init2D();
}

void Basic::Update(int delta)
{
}
void Basic::GetRequireScreenSize(int32_t &width, int32_t &height)
{
	width = Screen_width;
	height = Screen_height;
}
bool Basic::OnGameKeyPressed(int key, int scancode, int action, int mods)
{
	char c = (char)key;
	//LOGI("Key: %d = %c\n",key, c);
	glm::mat4 cameraView = mCamera->GetView();
	
	
	glm::mat4 tmp;
	if (action == 0) return true;
	switch (c)
	{
	case 'B':
		RenderManager::getInstance()->SwitchBloomMode();
		return true;
	case 'M':
		RenderManager::getInstance()->SwitchShadowMapMode();
		return true;
	case 'P':
		PhysicsSimulation::getInstance()->SwitchDebugMode();
		return true;
	case 'S': //num s
		cameraView = glm::rotate(cameraView, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mCamera->SetView(cameraView);
		return true;
	case 'W': //num w
		cameraView = glm::rotate(cameraView, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mCamera->SetView(cameraView);
		return true;
	case 'A': //num a
		return true;
	case 'D': //num d

		return true;
	case 262:
		return true;
	case 320: //num 0

		return true;
	case 321:

		return true;
	case 322:

		return true;
	case 323:

		return true;
	case 324:

		return true;
	case 325:

		return true;
	case 326:

		return true;
	case 327:

		return true;
	case 328:

		return true;
	case 329: // num 9

		return true;

	default:
		break;
	}
	return true;
}

bool Basic::OnGameTouchEvent(int eventId, int x, int y, int pointerId)
{
	return false;
}

void Basic::OnGameLoadingThreadFinished(int loadingtimeinms)
{
	mShadowLabel.setVisible(true);
	mBloomLabel.setVisible(true);
	mBloomAmountLabel.setVisible(true);
}

void Basic::ClickbuttonBloomAmountLeft()
{
	int bloomamount = RenderManager::getInstance()->GetAmountBloom();
	if (bloomamount >= 2)
		bloomamount -= 2;
	RenderManager::getInstance()->SetAmountBloom(bloomamount);
	mBloomAmountLabel.setText("Bloom Amount:       %d", RenderManager::getInstance()->GetAmountBloom());
	mbtBloomAmount1->SetPos(mBloomAmountLabel.getEndPos_x() + 10, 540 - mbtSwitchBloom->GetHeight() - 8);
}

void Basic::ClickbuttonBloomAmountRight()
{
	int bloomamount = RenderManager::getInstance()->GetAmountBloom();
	bloomamount += 2;
	RenderManager::getInstance()->SetAmountBloom(bloomamount);
	mBloomAmountLabel.setText("Bloom Amount:       %d", RenderManager::getInstance()->GetAmountBloom());
	mbtBloomAmount1->SetPos(mBloomAmountLabel.getEndPos_x() + 10, 540 - mbtSwitchBloom->GetHeight() - 8);
}

void Basic::ClickbuttonShadow()
{
	RenderManager::getInstance()->SwitchShadowMapMode();
	bool isEnableShadow = RenderManager::getInstance()->IsEnableShadow();
	mbtSwitchShadow->SetGrayOut(!isEnableShadow);
	mShadowLabel.setText("Shadow:   %s", isEnableShadow ? "ON" : "OFF");
}

void Basic::ClickbuttonBloom()
{
	RenderManager::getInstance()->SwitchBloomMode();
	bool isEnableBloom = RenderManager::getInstance()->IsEnableBloom();
	mbtSwitchBloom->SetGrayOut(!isEnableBloom);
	mBloomLabel.setText("Bloom:   %s", isEnableBloom ? "ON" : "OFF");
}

void Basic::Init2D()
{
	bool isEnableShadow = RenderManager::getInstance()->IsEnableShadow();
	mShadowLabel.setText("Shadow:   %s", isEnableShadow ? "ON" : "OFF");
	mShadowLabel.setScale(0.5f);
	mShadowLabel.setPos(5, Screen_height - 30);
	mShadowLabel.setVisible(false);

	mbtSwitchShadow = UserInterface::CreateWithTexture("button/buttons_PNG126.png");
	mbtSwitchShadow->SetCallbackOnTouchBegan(std::bind(&Basic::ClickbuttonShadow, this));
	mbtSwitchShadow->SetScale(1.2f);
	mbtSwitchShadow->SetPos(100, Screen_height - mbtSwitchShadow->GetHeight() - 8);
	mbtSwitchShadow->SetGrayOut(!isEnableShadow);

	bool isEnableBloom = RenderManager::getInstance()->IsEnableBloom();
	mBloomLabel.setText("Bloom:   %s", isEnableBloom ? "ON" : "OFF");
	mBloomLabel.setScale(0.5f);
	mBloomLabel.setPos(170, Screen_height - 30);
	mBloomLabel.setVisible(false);

	mbtSwitchBloom = UserInterface::CreateWithTexture("button/buttons_PNG126.png");
	mbtSwitchBloom->SetCallbackOnTouchBegan(std::bind(&Basic::ClickbuttonBloom, this));
	mbtSwitchBloom->SetScale(1.2f);
	mbtSwitchBloom->SetPos(250, Screen_height - mbtSwitchBloom->GetHeight() - 8);
	mbtSwitchBloom->SetGrayOut(!isEnableBloom);

	int bloomamount = RenderManager::getInstance()->GetAmountBloom();

	mBloomAmountLabel.setText("Bloom Amount:       %d", bloomamount);
	mBloomAmountLabel.setScale(0.5f);
	mBloomAmountLabel.setPos(330, Screen_height - 30);
	mBloomAmountLabel.setVisible(false);

	mbtBloomAmount1 = UserInterface::CreateWithTexture("button/button_right.png");
	mbtBloomAmount1->SetCallbackOnTouchBegan(std::bind(&Basic::ClickbuttonBloomAmountRight, this));
	mbtBloomAmount1->SetScale(0.017f);
	mbtBloomAmount1->SetPos(mBloomAmountLabel.getEndPos_x() + 10, Screen_height - mbtSwitchBloom->GetHeight() - 8);

	mbtBloomAmount2 = UserInterface::CreateWithTexture("button/button_left.png");
	mbtBloomAmount2->SetCallbackOnTouchBegan(std::bind(&Basic::ClickbuttonBloomAmountLeft, this));
	mbtBloomAmount2->SetScale(0.017f);
	mbtBloomAmount2->SetPos(490, Screen_height - mbtSwitchBloom->GetHeight() - 8);
}
Basic::Basic()
{
	m_initialized = false;
}

Basic::~Basic()
{
	delete mbtSwitchShadow;
	delete mbtSwitchBloom;
}
AppBase* AppFactory()
{
	return new Basic();
}