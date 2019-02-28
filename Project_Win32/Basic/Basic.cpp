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

	//mNanosuit.Init("nanosuit/nanosuit.obj", true, false);
	//mNanosuit.SetScale(glm::vec3(0.4f));
	//mNanosuit.SetTranslate(glm::vec3(9.0f, 3.0f, 0.0f));
	//mNanosuit.SetDrawPolygon(true);

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("Streetenvironment/Street environment_V01.obj", true, false);
	m_Streetenvironment.SetTranslate(glm::vec3(0.0f, -0.03f, 0.5f));
	m_Streetenvironment.SetIsDrawDepthMap(false);

	//mMerce.Init("MercedesBenzSLSAMG/sls_amg.obj", true);
	mMerce.SetRotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	mMerce.SetTranslate(glm::vec3(0.0f, 0.0f, 1.2f));
	mMerce.SetScale(glm::vec3(2.5f));

	//mSpider.Init("aboy/model.dae", true, true);
	//mSpider.Init("Simple.dae", true);
	//mSpider.Init("boblampclean/boblampclean.md5mesh", true);
	mSpider.Init("Low-Poly Spider/Spider.fbx", true);
	mSpider.SetScale(glm::vec3(0.2f));
	//mSpider.SetTranslate(glm::vec3(0.0f, 1.0f, 0.0f));
	mSpider.SetAnimPlay(0);
	mSpider.SetNeedRotate(true);
	//mSpider.SetDrawMesh(0);
	//saberclass.Init("test/untitled.obj", false, 3.0f);
	//saberclass.SetTranslate(glm::vec3(0.0f, 3.0f, -20.0f));
	//mGallacticCruiser.Init("GallacticCruiser/Class II Gallactic Cruiser.obj", false, 0.1f);
	//mGallacticCruiser.SetTranslate(glm::vec3(-10.0f, -3.0f, 0.0f));

	mMonster_1.Init("boblampclean/boblampclean.md5mesh", true);
	//mMonster_1.Init("/astroBoy/AnimSimple.dae", true);
	//mMonster_1.Init("aboy/model.dae", true);
	//mMonster_1.SetRotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	mMonster_1.SetTranslate(glm::vec3(0.0f, 0.0f, 0.0f));
	mMonster_1.SetScale(glm::vec3(0.2f));
	mMonster_1.SetNeedRotate(true);

	//soundIntro.Init("Sound/chuabaogio.wav");
	//soundIntro.Play();
	axis.Init(mCamera);
	m_initialized = true;
	//RenderManager::getInstance()->SetEnableShadowMap(false);
}

void Basic::Update()
{
	
}
void Basic::GetRequireScreenSize(int32_t &width, int32_t &height)
{
	width = 960;
	height = 540;
}
void Basic::OnGameKeyPressed(int key, int scancode, int action, int mods)
{
	char c = (char)key;
	//LOGI("Key: %d = %c\n",key, c);
	if (action == 0) return;
	switch (c)
	{
	case 'B':
		RenderManager::getInstance()->SwitchBloomMode();
		return;
	case 'M':
		RenderManager::getInstance()->SwitchShadowMapMode();
		return;
	case 'S': //num s
		mCamera->Target.y -= 0.1;
		return;
	case 'W': //num w
		mCamera->Target.y += 0.1;
		return;
	case 'A': //num a
		mCamera->Target.x -= 0.1;
		return;
	case 'D': //num d
		mCamera->Target.x += 0.1;
		return;
	case 262:
		timestamp_for_lamp += 1;
		return;
	case 320: //num 0
		mSpider.SetAnimPlay(0);
		return;
	case 321:
		mSpider.SetAnimPlay(1);
		return;
	case 322:
		mSpider.SetAnimPlay(2);
		return;
	case 323:
		mSpider.SetAnimPlay(3);
		return;
	case 324:
		mSpider.SetAnimPlay(4);
		return;
	case 325:
		mSpider.SetAnimPlay(5);
		return;
	case 326:
		mSpider.SetAnimPlay(6);
		return;
	case 327:
		mSpider.SetAnimPlay(7);
		return;
	case 328:
		mSpider.SetAnimPlay(8);
		return;
	case 329: // num 9
		mSpider.SetAnimPlay(9);
		return;

	default:
		break;
	}
}
Basic::Basic()
{
	timestamp_for_lamp = 0;
	m_initialized = false;
	mCamera = Camera::getInstance();
}

Basic::~Basic()
{
}
AppBase* AppFactory()
{
	return new Basic();
}