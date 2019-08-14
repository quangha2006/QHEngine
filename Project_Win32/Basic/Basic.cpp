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


void Basic::Init()
{
	mCamera->Pos = glm::vec3(5.0f, 5.0f, 20.0f);
	mCamera->Target = glm::vec3(0.0f, 1.0f, 0.0f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	mCamera->lightPos = glm::vec3(8.2f, 10.0f, 9.0f);

	mNanosuit.Init("nanosuit/nanosuit.obj", true, false);
	mNanosuit.SetScale(glm::vec3(0.4f));
	mNanosuit.SetPos(glm::vec3(9.0f, 0.0f, 0.0f));
	mNanosuit.SetDrawPolygon(true);

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("Streetenvironment/Street environment_V01.obj", true, false);
	m_Streetenvironment.SetPos(glm::vec3(0.0f, -0.03f, 0.5f));
	m_Streetenvironment.SetIsDrawDepthMap(false);
	m_Streetenvironment.CreateBoxShapePhysicsBody(0.0f, glm::vec3(48.0, .1, 48.0), glm::vec3(0., 0.001, 0.));
	m_Streetenvironment.GetRigidBody()->setFriction(0.0);
	m_Streetenvironment.GetRigidBody()->setRestitution(1.);

	//mMerce.Init("MercedesBenzSLSAMG/sls_amg.obj", true);
	mMerce.SetRotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	mMerce.SetPos(glm::vec3(0.0f, 1.2f, 1.2f));
	mMerce.SetScale(glm::vec3(2.5f));

	mAstroBoy.Init("astroBoy/astroBoy_walk_Max.dae", true, true);
	mAstroBoy.SetRotate(180.0, glm::vec3(.0f, 1.0f, .0f));
	mAstroBoy.SetPos(glm::vec3(-1.0f, 0.0f, 0.0f));

	mSpider.Init("Low-Poly Spider/Spider_3.fbx", true);
	mSpider.SetRotate(180.0, glm::vec3(.0f, 1.0f, .0f));
	mSpider.SetScale(glm::vec3(0.05f));
	mSpider.SetPos(glm::vec3(-8.0f, 0.0f, 0.0f));
	mSpider.SetAnimPlay(2);
	//mSpider.SetTimeStampAnim(0);
	mSpider.SetNeedRotate(true);
	//mSpider.SetDrawMesh(0);

	mBoblampclean.Init("boblampclean/boblampclean.md5mesh", true);
	mBoblampclean.SetRotate(180.0, glm::vec3(.0f, 1.0f, .0f));
	mBoblampclean.SetPos(glm::vec3(3.5f, 0.0f, 0.0f));
	mBoblampclean.SetScale(glm::vec3(0.1f));
	mBoblampclean.SetNeedRotate(true);
	mBoblampclean.SetUseLighting(false);

	uvcircle.Init("3DBreakOutGame/UVCircle.dae");
	uvcircle.SetScale(glm::vec3(1.0));
	uvcircle.SetPos(glm::vec3(0.0f, 5.0f, 10.0f));

	//soundIntro.Init("Sound/chuabaogio.wav");
	//soundIntro.Play();
	axis.Init(mCamera);
	m_initialized = true;
	//RenderManager::getInstance()->SetEnableShadowMap(false);
	RenderManager::getInstance()->SetEnableBloom(false);

	bool isEnableShadow = RenderManager::getInstance()->IsEnableShadow();
	mShadowLabel.setText("Shadow:   %s", isEnableShadow ? "ON" : "OFF");
	mShadowLabel.setScale(0.5f);
	mShadowLabel.setPos(5, 540 - 30);
	mShadowLabel.setVisible(false);

	mbtSwitchShadow = UserInterface::CreateWithTexture("button/buttons_PNG126.png");
	mbtSwitchShadow->SetCallbackOnTouchBegan(std::bind(&Basic::ClickbuttonShadow, this));
	mbtSwitchShadow->SetScale(1.2f);
	mbtSwitchShadow->SetPos(100, 540 - mbtSwitchShadow->GetHeight() - 8);
	mbtSwitchShadow->SetGrayOut(!isEnableShadow);
	
	bool isEnableBloom = RenderManager::getInstance()->IsEnableBloom();
	mBloomLabel.setText("Bloom:   %s", isEnableBloom ? "ON" : "OFF");
	mBloomLabel.setScale(0.5f);
	mBloomLabel.setPos(170, 540 - 30);
	mBloomLabel.setVisible(false);
	
	mbtSwitchBloom = UserInterface::CreateWithTexture("button/buttons_PNG126.png");
	mbtSwitchBloom->SetCallbackOnTouchBegan(std::bind(&Basic::ClickbuttonBloom, this));
	mbtSwitchBloom->SetScale(1.2f);
	mbtSwitchBloom->SetPos(250, 540 - mbtSwitchBloom->GetHeight() - 8);
	mbtSwitchBloom->SetGrayOut(!isEnableBloom);
	
	int bloomamount = RenderManager::getInstance()->GetAmountBloom();

	mBloomAmountLabel.setText("Bloom Amount:       %d", bloomamount);
	mBloomAmountLabel.setScale(0.5f);
	mBloomAmountLabel.setPos(330, 540 - 30);
	mBloomAmountLabel.setVisible(false);

	mbtBloomAmount1 = UserInterface::CreateWithTexture("button/button_right.png");
	mbtBloomAmount1->SetCallbackOnTouchBegan(std::bind(&Basic::ClickbuttonBloomAmountRight, this));
	mbtBloomAmount1->SetScale(0.017f);
	mbtBloomAmount1->SetPos(mBloomAmountLabel.getEndPos_x() + 10, 540 - mbtSwitchBloom->GetHeight() - 8);

	mbtBloomAmount2 = UserInterface::CreateWithTexture("button/button_left.png");
	mbtBloomAmount2->SetCallbackOnTouchBegan(std::bind(&Basic::ClickbuttonBloomAmountLeft, this));
	mbtBloomAmount2->SetScale(0.017f);
	mbtBloomAmount2->SetPos(490, 540 - mbtSwitchBloom->GetHeight() - 8);
}

void Basic::Update(int delta)
{

}
void Basic::GetRequireScreenSize(int32_t &width, int32_t &height)
{
	width = 960;
	height = 540;
}
bool Basic::OnGameKeyPressed(int key, int scancode, int action, int mods)
{
	char c = (char)key;
	//LOGI("Key: %d = %c\n",key, c);
	if (action == 0) return true;
	switch (c)
	{
	case 'B':
		RenderManager::getInstance()->SwitchBloomMode();
		return true;
	case 'M':
		RenderManager::getInstance()->SwitchShadowMapMode();
		return true;
	case 'S': //num s
		mCamera->Target.y -= 0.1f;
		return true;
	case 'W': //num w
		mCamera->Target.y += 0.1f;
		return true;
	case 'A': //num a
		mCamera->Target.x -= 0.1f;
		return true;
	case 'D': //num d
		mCamera->Target.x += 0.1f;
		return true;
	case 262:
		timestamp_for_lamp += 1.f;
		return true;
	case 320: //num 0
		mSpider.SetAnimPlay(0);
		return true;
	case 321:
		mSpider.SetAnimPlay(1);
		return true;
	case 322:
		mSpider.SetAnimPlay(2);
		return true;
	case 323:
		mSpider.SetAnimPlay(3);
		return true;
	case 324:
		mSpider.SetAnimPlay(4);
		return true;
	case 325:
		mSpider.SetAnimPlay(5);
		return true;
	case 326:
		mSpider.SetAnimPlay(6);
		return true;
	case 327:
		mSpider.SetAnimPlay(7);
		return true;
	case 328:
		mSpider.SetAnimPlay(8);
		return true;
	case 329: // num 9
		mSpider.SetAnimPlay(9);
		return true;

	default:
		break;
	}
	return true;
}

bool Basic::OnGameTouchEvent(int eventId, int x, int y, int pointerId)
{
	//mbutton->SetPos(x, y);
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
Basic::Basic()
{
	timestamp_for_lamp = 0;
	m_initialized = false;
	mCamera = Camera::getInstance();
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