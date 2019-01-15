#include "Basic.h"
#include "Logs.h"
#include "TextRendering.h"
#include "FrameRate.h"
#include "Timer.h"
#include "Debugging.h"
#include "QHText.h"
#include "SkyBox.h"
#include "Utils.h"

void Basic::Init()
{
	//mShadowBuffer.Init(mContext, FrameBufferType_DEPTH, 2048, 2048);
	////mframebuffer.EnableDebug(true);
	//mSenceBuffer.Init(mContext, FrameBufferType_COLORBUFFER_MULTISAMPLED, mContext->GetWindowWidth(), mContext->GetWindowHeight());

	////mSenceBuffer.Init(mContext, FrameBufferType_COLORBUFFER, mContext->GetWindowWidth(), mContext->GetWindowHeight());

	//mBrightnessBuffer.Init(mContext, FrameBufferType_COLORBUFFER, mContext->GetWindowWidth(), mContext->GetWindowHeight());

	//bluringBuffer.Init(mContext, FrameBufferType_COLORBUFFER_BLURRING, mContext->GetWindowWidth(), mContext->GetWindowHeight());

	AppSharedContext *shared_context = mContext->CreateShareContext();
	new thread(&Basic::LoadingThread, this, shared_context);
}

void Basic::LoadingThread(AppSharedContext *shared_context)
{
	shared_context->MakeContextCurrent();
	

	loadingText.setPos(mContext->GetWindowWidth()/2-150, mContext->GetWindowHeight()/2);
	loadingText.setText(Utils::toString("Loading %d%c", 0, 37));
	
	mCamera->Pos = glm::vec3(5.0f, 5.0f, 20.0f);
	mCamera->Target = glm::vec3(0.0f, 1.0f, 0.0f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	mCamera->lightPos = glm::vec3(8.2f, 10.0f, 9.0f);

	loadingText.setText(Utils::toString("Loading %d%c", 5, 37));

	ShaderManager::getInstance()->Init("model","Shaders/model_loading.vs" ,"Shaders/model_loading.fs");
	ShaderManager::getInstance()->Init("debugShader", "Shaders/framebuffers_debug.vs", "Shaders/framebuffers_debug.fs"); // For debug
	ShaderManager::getInstance()->Init("depthShader", "Shaders/DepthShader.vs", "Shaders/DepthShader.fs");
	ShaderManager::getInstance()->Init("basic", "Shaders/BasicVS.vs", "Shaders/BasicFS.fs");
	ShaderManager::getInstance()->Init("Brightness", "Shaders/BasicVS.vs", "Shaders/brightness.fs");
	ShaderManager::getInstance()->Init("blur", "Shaders/BasicVS.vs", "Shaders/blur.fs");
	ShaderManager::getInstance()->Init("bloom_Final", "Shaders/BasicVS.vs", "Shaders/bloom_final.fs");
	//mNanosuit.Init("Light Bulb/Light Bulb 1.dae", mCamera, false);
	//mNanosuit.SetScale(glm::vec3(0.4f));
	//mNanosuit.SetTranslate(glm::vec3(9.0f, 3.0f, 0.0f));
	//mNanosuit.SetDrawPolygon(true);

	loadingText.setText(Utils::toString("Loading %d%c", 10, 37));

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("Streetenvironment/Street environment_V01.obj", mCamera, true);
	m_Streetenvironment.SetTranslate(glm::vec3(0.0f, -0.03f, 0.5f));

	//mMerce.Init("MercedesBenzSLSAMG/sls_amg.obj", mCamera, true);
	mMerce.SetRotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	mMerce.SetTranslate(glm::vec3(0.0f, 0.0f, 1.2f));
	mMerce.SetScale(glm::vec3(2.5f));

	loadingText.setText(Utils::toString("Loading %d%c", 20, 37));
	//mSpider.Init("Low-Poly Spider/Only_Spider_with_Animations_Export.dae", mCamera, true);
	//mSpider.Init("Simple.dae", mCamera, true);
	//mSpider.Init("boblampclean/boblampclean.md5mesh", mCamera, true);
	mSpider.Init("Frog/source.dae", mCamera, true);
	//mSpider.SetScale(glm::vec3(0.42f));
	mSpider.SetTranslate(glm::vec3(0.0f, 1.0f, 0.0f));
	mSpider.SetAnimPlay(0);
	mSpider.SetNeedRotate(false);

	loadingText.setText(Utils::toString("Loading %d%c", 40,37));
	//saberclass.Init("test/untitled.obj", mCamera, false, 3.0f);
	//saberclass.SetTranslate(glm::vec3(0.0f, 3.0f, -20.0f));
	//mGallacticCruiser.Init("GallacticCruiser/Class II Gallactic Cruiser.obj", mCamera, false, 0.1f);
	//mGallacticCruiser.SetTranslate(glm::vec3(-10.0f, -3.0f, 0.0f));

	//mMonster_1.Init("boblampclean/boblampclean.md5mesh", mCamera, false);
	//mMonster_1.Init("/astroBoy/AnimSimple.dae", mCamera, true);
	//mMonster_1.Init("aboy/model.dae", mCamera, true);
	//mMonster_1.SetRotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	mMonster_1.SetTranslate(glm::vec3(0.0f, 0.0f, 0.0f)); 
	//mMonster_1.SetScale(glm::vec3(5.02f));
	mMonster_1.SetNeedRotate(true);
	loadingText.setText(Utils::toString("Loading %d%c", 80,37));
	
	//soundIntro.Init("Sound/chuabaogio.wav");
	//soundIntro.Play();
	loadingText.setText(Utils::toString("Loading %d%c", 100, 37));
	axis.Init(mCamera);
	loadingText.visible = false;
	shared_context->DestroyContext();
	m_initialized = true;
	delete(shared_context);
}

void Basic::Update()
{
	if (!m_initialized)
		return;
	
	/*mSpider.Update();
	mMerce.Update();
	mMonster_1.Update();*/
	
	/*mShadowBuffer.Enable("depthShader");
	ShaderManager::getInstance()->setMat4("lightSpaceMatrix", mCamera->lightSpaceMatrix);
	ShaderManager::getInstance()->setFloat("near_plane", mCamera->light_near);
	ShaderManager::getInstance()->setFloat("far_plane", mCamera->light_far);

	mNanosuit.Render();
	mMerce.Render();
	mSpider.Render();
	saberclass.Render();
	mGallacticCruiser.Render();
	mMonster_1.Render();

	GLuint depthMapRT = mShadowBuffer.Disable();

	mSenceBuffer.Enable("model");
	mSkyBox.Draw(mCamera);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, depthMapRT);
	ShaderManager::getInstance()->setBool("useShadowMap", true);
	ShaderManager::getInstance()->setInt("shadowMap",10);

	m_Streetenvironment.Render();
	mNanosuit.Render();
	mMerce.Render();
	mSpider.Render();
	saberclass.Render();
	mGallacticCruiser.Render();
	mMonster_1.Render();
	GLuint SenceRT = mSenceBuffer.Disable();

	mBrightnessBuffer.Enable();
	ShaderManager::getInstance()->setUseProgram("Brightness");
	mSenceBuffer.Render();
	GLuint brightnessRT = mBrightnessBuffer.Disable();
	ShaderManager::getInstance()->setUseProgram("blur");

	bluringBuffer.MakeBlur(SenceRT, brightnessRT);*/
	//axis.Draw();
	//Debugging::getInstance()->DrawTex(depthMapRT, "debugShader");
}
void Basic::GetRequireScreenSize(int32_t &width, int32_t &height)
{
	width = 900;
	height = 540;
	//width = 1920;
	//height = 1024;
}
void Basic::OnGameKeyPressed(int key, int scancode, int action, int mods)
{
	//LOGI("Key: %d", key);
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
}

Basic::~Basic()
{
}
AppBase* AppFactory()
{
	return new Basic();
}