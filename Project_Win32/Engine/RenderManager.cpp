#include "RenderManager.h"

RenderManager * RenderManager::instance = NULL;


RenderManager * RenderManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new RenderManager();
	}
	return instance;
}

void RenderManager::Init(AppContext * appcontext, Camera *camera)
{
	mAppcontext = appcontext;
	mCamera = camera;

	mShadowRT.Init(mAppcontext, RenderTargetType_DEPTH, 2048, 2048);

	mSenceRT.Init(mAppcontext, RenderTargetType_COLOR_MULTISAMPLED, mAppcontext->GetWindowWidth(), mAppcontext->GetWindowHeight());

	//mSenceBuffer.Init(mContext, FrameBufferType_COLORBUFFER, mContext->GetWindowWidth(), mContext->GetWindowHeight());

	mBrightnessRT.Init(mAppcontext, RenderTargetType_COLOR, mAppcontext->GetWindowWidth(), mAppcontext->GetWindowHeight());

	mBluringRT.Init(mAppcontext, RenderTargetType_COLOR_BLURRING, mAppcontext->GetWindowWidth(), mAppcontext->GetWindowHeight());
}

void RenderManager::Update()
{
	ModelManager::getInstance()->Update();
}

void RenderManager::Render()
{
	mShadowRT.Enable("depthShader");
	ShaderManager::getInstance()->setMat4("lightSpaceMatrix", mCamera->lightSpaceMatrix);
	ShaderManager::getInstance()->setFloat("near_plane", mCamera->light_near);
	ShaderManager::getInstance()->setFloat("far_plane", mCamera->light_far);

	/*mNanosuit.Render();
	mMerce.Render();
	mSpider.Render();
	saberclass.Render();
	mGallacticCruiser.Render();
	mMonster_1.Render();*/
	ModelManager::getInstance()->Render();
	GLuint depthMapRT = mShadowRT.Disable();

	mSenceRT.Enable("model");
	//mSkyBox.Draw(mCamera);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, depthMapRT);
	ShaderManager::getInstance()->setBool("useShadowMap", true);
	ShaderManager::getInstance()->setInt("shadowMap", 10);

	//m_Streetenvironment.Render();
	//mNanosuit.Render();
	//mMerce.Render();
	//mSpider.Render();
	//saberclass.Render();
	//mGallacticCruiser.Render();
	//mMonster_1.Render();
	ModelManager::getInstance()->Render();
	GLuint SenceRT = mSenceRT.Disable();

	mBrightnessRT.Enable();
	ShaderManager::getInstance()->setUseProgram("Brightness");
	mSenceRT.Render();
	GLuint brightnessRT = mBrightnessRT.Disable();
	ShaderManager::getInstance()->setUseProgram("blur");

	mBluringRT.MakeBlur(SenceRT, brightnessRT);
}

RenderManager::RenderManager()
{
}


RenderManager::~RenderManager()
{
}
