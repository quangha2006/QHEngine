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


void Clickbutton()
{
	RenderManager::getInstance()->SwitchShadowMapMode();
}

void Basic::Init()
{
	mCamera->Pos = glm::vec3(5.0f, 5.0f, 20.0f);
	mCamera->Target = glm::vec3(0.0f, 1.0f, 0.0f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	mCamera->lightPos = glm::vec3(8.2f, 10.0f, 9.0f);

	//mNanosuit.Init("nanosuit/nanosuit.obj", true, false);
	//mNanosuit.SetScale(glm::vec3(0.4f));
	//mNanosuit.SetTranslate(glm::vec3(9.0f, 3.0f, 0.0f));
	//mNanosuit.SetDrawPolygon(true);

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("Streetenvironment/Street environment_V01.obj", true, false);
	m_Streetenvironment.SetPos(glm::vec3(0.0f, -0.03f, 0.5f));
	m_Streetenvironment.SetIsDrawDepthMap(false);

	//mMerce.Init("MercedesBenzSLSAMG/sls_amg.obj", true);
	mMerce.SetRotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	mMerce.SetPos(glm::vec3(0.0f, 1.2f, 1.2f));
	mMerce.SetScale(glm::vec3(2.5f));

	//mSpider.Init("aboy/model.dae", true, true);
	//mSpider.Init("Simple.dae", true);
	//mSpider.Init("boblampclean/boblampclean.md5mesh", true);
	mSpider.Init("Low-Poly Spider/Spider_3.fbx", true);
	mSpider.SetScale(glm::vec3(0.05f));
	mSpider.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
	mSpider.SetAnimPlay(0);
	//mSpider.SetTimeStampAnim(0);
	mSpider.SetNeedRotate(true);
	//mSpider.SetDrawMesh(0);
	//saberclass.Init("test/untitled.obj", false, 3.0f);
	//saberclass.SetTranslate(glm::vec3(0.0f, 3.0f, -20.0f));
	//mGallacticCruiser.Init("GallacticCruiser/Class II Gallactic Cruiser.obj", false, 0.1f);
	//mGallacticCruiser.SetTranslate(glm::vec3(-10.0f, -3.0f, 0.0f));

	//mMonster_1.Init("boblampclean/boblampclean.md5mesh", true);
	//mMonster_1.Init("/astroBoy/AnimSimple.dae", true);
	//mMonster_1.Init("aboy/model.dae", true);
	//mMonster_1.SetRotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	mMonster_1.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
	mMonster_1.SetScale(glm::vec3(0.2f));
	mMonster_1.SetNeedRotate(true);

	uvcircle.Init("3DBreakOutGame/UVCircle.dae");
	uvcircle.SetScale(glm::vec3(3.0));
	uvcircle.SetPos(glm::vec3(0.0f, 0.0f, 10.0f));

	//soundIntro.Init("Sound/chuabaogio.wav");
	//soundIntro.Play();
	axis.Init(mCamera);
	m_initialized = true;
	//RenderManager::getInstance()->SetEnableShadowMap(false);
	RenderManager::getInstance()->SetEnableBloom(false);

	mbutton = UserInterface::CreateWithTexture("button/CloseNormal.png");
	//mbutton->SetCallback(extiLog);
	mbutton->SetPos(mContext->GetWindowWidth() - mbutton->getWidth(), mContext->GetWindowHeight() - mbutton->getHeight());

	mbtSwitchShadow = UserInterface::CreateWithTexture("button/buttons_PNG126.png");
	mbtSwitchShadow->SetCallbackOnTouchBegan(Clickbutton);
	mbtSwitchShadow->SetPos(0, 100);
	mbtSwitchShadow->SetScale(2.0f);
	mbtSwitchShadow->SetAlpha(0.5f);
}

void Basic::Update(int delta)
{
	//mSpider.SetRotate(Timer::getMillisecond()/100,glm::vec3(1.0f, 0.0f, 0.0f));
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
		mCamera->Target.y -= 0.1;
		return true;
	case 'W': //num w
		mCamera->Target.y += 0.1;
		return true;
	case 'A': //num a
		mCamera->Target.x -= 0.1;
		return true;
	case 'D': //num d
		mCamera->Target.x += 0.1;
		return true;
	case 262:
		timestamp_for_lamp += 1;
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

Basic::Basic()
{
	timestamp_for_lamp = 0;
	m_initialized = false;
	mCamera = Camera::getInstance();
}

Basic::~Basic()
{
	delete mbutton;
}
AppBase* AppFactory()
{
	return new Basic();
}