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

const int Screen_width = 960;
const int Screen_height = 540;

void Basic::Init()
{
	mCamera->SetPos(0.0f, 15.0f, 40.0f);
	mCamera->SetTarget(0.0f, 5.0f, 0.0f);
	mCamera->SetLightPos(0.2f, 40.0f, 1.0f);

	//mNanosuit.Init("nanosuit/nanosuit.obj", true, false);
	mNanosuit.SetScale(glm::vec3(0.4f));
	mNanosuit.SetPos(glm::vec3(9.0f, 0.0f, 0.0f));
	mNanosuit.SetDrawPolygon(true);

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("GameDemo/terrain.dae", true, true);
	m_Streetenvironment.SetPos(glm::vec3(0.0f, -20.5f, 0.0f));
	m_Streetenvironment.SetScale(glm::vec3(40.0f,20.0f,40.0f));
	m_Streetenvironment.SetIsDrawDepthMap(false);
	//m_Streetenvironment.CreateBoxShapePhysicsBody(0.0f, glm::vec3(48.0, 1., 48.0), glm::vec3(0., -0.48, 0.));
	//m_Streetenvironment.GetRigidBody()->setFriction(0.0);
	//m_Streetenvironment.GetRigidBody()->setRestitution(1.);

	//btRigidBody* body = PhysicsSimulation::getInstance()->createBoxShape(0.0f, glm::vec3(-1.2f, 16.0f, 0.0f), glm::vec3(1.f), 0.0f, glm::vec3(1), glm::vec3(1.0, 4.0f, 1.0f));
	//body->setFriction(0.0);
	//body->setRestitution(1.);
	//btRigidBody* body1 = PhysicsSimulation::getInstance()->createBoxShape(1.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.f), 0.0f, glm::vec3(8));
	//body1->setFriction(0.0);
	//body1->setRestitution(1.);
	//btRigidBody* body2 = PhysicsSimulation::getInstance()->createCapsuleShape(2.f, 16.0f, 1.0f, glm::vec3(0.0f, 16.0f, 0.0f));
	//body2->setFriction(1.0);
	//body2->setRestitution(1.);
	//body2->setRollingFriction(.5f);
	//btRigidBody* body3 = PhysicsSimulation::getInstance()->createCapsuleShape(1.f, 1.0f, 2.0f, glm::vec3(4.0f, 16.0, 0));
	//body3->getCollisionShape();
	//body3->setFriction(1.0);
	//body3->setRestitution(1.);
	//body3->setRollingFriction(1.0f);

	//mMerce.Init("MercedesBenzSLSAMG/sls_amg.obj", true);
	//mMerce.Init("MercedesBenzSLSAMG/MercedesBenzSLSAMG.dae", true);
	mMerce.SetRotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	mMerce.SetPos(glm::vec3(7.0f, 1.2f, 1.2f));
	mMerce.SetScale(glm::vec3(2.5f));

	mAstroBoy.Init("bountyhunter/bountyhunter/export_from_max/test.FBX", true);
	//mAstroBoy.SetRotate(180.0, glm::vec3(.0f, 1.0f, .0f));
	mAstroBoy.SetScale(glm::vec3(0.1f));
	mAstroBoy.SetPos(glm::vec3(-8.0f, 0.0f, 0.0f));
	mAstroBoy.SetDrawMesh(1);
	mAstroBoy.CreateBoxShapePhysicsBody(1.0, glm::vec3(48.0, 1., 48.0));
	//mAstroBoy.SetPlayAnimTime(0.f, .33f);

	//mSpider.Init("Low-Poly Spider/Spider_3.fbx", true);
	//mSpider.Init("bHieu/1_2_Tree.dae", true);
	//mSpider.Init("Demo/BoxAnim.dae", true);
	//mSpider.SetScale(glm::vec3(0.004f));
	mSpider.SetPos(glm::vec3(1.8f, 35.5f, -.5f));
	//mSpider.SetScale(glm::vec3(5.0f));
	//mSpider.SetAnimPlay(1);
	//mSpider.SetRotate(50.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	//mSpider.SetTimeStampAnim(0);
	//mSpider.SetNeedRotate(true);
	//mSpider.SetDrawMesh(9);
	//mSpider.SetPlayAnimTime(0.0f, 0.33f);
	mSpider.SetPlayAnimTime(0.66f, 1.f);

	//mBoblampclean.Init("bountyhunter/bountyhunter/export_from_max/test.FBX", true);
	mBoblampclean.SetPos(glm::vec3(5.f, 0.0f, 0.0f));
	mBoblampclean.SetScale(glm::vec3(0.05f));
	mBoblampclean.SetDrawMesh(1);

	uvcircle.Init("3DBreakOutGame/UVCircle2.dae");
	//uvcircle.SetScale(glm::vec3(1.0, 0.5f, 1.0f));
	uvcircle.SetPos(glm::vec3(0.1f, 5.f, 5.5f));

	uvcircle.CreateSphereShapePhysicsBody(1., 1.);

	uvcircle.GetRigidBody()->setFriction(1.);
	uvcircle.GetRigidBody()->setRollingFriction(1.);
	uvcircle.GetRigidBody()->setSpinningFriction(1.);
	uvcircle.GetRigidBody()->setRestitution(1.);
	//uvcircle.GetRigidBody()->applyForce(btVector3(100.0, 0., 0.), btVector3(0.0, 0.0, 0.0));

	axis.Init(mCamera);
	m_initialized = true;
	//RenderManager::getInstance()->SetEnableShadowMap(false);
	RenderManager::getInstance()->SetEnableBloom(true);
	PhysicsSimulation::getInstance()->SwitchDebugMode();
	//PhysicsSimulation::getInstance()->createTriangleMeshShape(0.0f, NULL, 0, NULL, 0, glm::vec3(), glm::vec3(), 1.0f, glm::vec3());

	Init2D();
}

void Basic::Update(int delta)
{
	
	//glm::mat4 trans = uvcircle.GetWorld();
	//Utils::PrintMat4(trans);
	//glm::vec3 circlePos = glm::vec3(trans[3][0], trans[3][1], trans[3][2]);
	//mCamera->SetTarget(circlePos);
	//mCamera->SetPos(circlePos.x + 50, circlePos.y + 30, circlePos.z + 50);
	//LOGI("%f, %f, %f\n", circlePos.z, circlePos.y, circlePos.z);
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
	glm::vec3 curent = uvcircle.GetScale();
	static glm::mat4 camera_projection = glm::mat4();
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
		m_Streetenvironment.SetRotate(1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		return true;
	case 'W': //num w
		m_Streetenvironment.SetRotate(-1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		return true;
	case 'A': //num a
		uvcircle.SetScale(glm::vec3(1.0, ++(curent.y), 1.0));
		return true;
	case 'D': //num d
		uvcircle.SetScale(glm::vec3(1.0, --(curent.y), 1.0));
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
	return false;
}

void Basic::OnGameLoadingThreadFinished(int loadingtimeinms)
{
	mShadowLabel.setVisible(true);
	mBloomLabel.setVisible(true);
	mBloomAmountLabel.setVisible(true);

	m_Streetenvironment.CreateTriangleMeshShape(0.);
	m_Streetenvironment.GetRigidBody()->setFriction(0.0);

	/*uvcircle.registerShape(1.);*/
	//uvcircle.GetRigidBody()->setFriction(0.);
	//uvcircle.GetRigidBody()->setRollingFriction(1.0f);
	//uvcircle.GetRigidBody()->setSpinningFriction(1.0f);
	//uvcircle.GetRigidBody()->setRestitution(0.0);

	/*mSpider.CreateConvexHullShapePhysicsBody(1., false);
	mSpider.GetRigidBody()->setFriction(0.);
	mSpider.GetRigidBody()->setRollingFriction(0.1f);
	mSpider.GetRigidBody()->setSpinningFriction(0.1f);
	mSpider.GetRigidBody()->setRestitution(0.0);*/
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
	timestamp_for_lamp = 0;
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