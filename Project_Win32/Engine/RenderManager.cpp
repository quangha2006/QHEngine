#include "RenderManager.h"
#include "Debugging.h"
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

	mShadowRT.Init(mAppcontext, RenderTargetType_DEPTH, 2048, 2048);

	//mSenceRT.Init(mAppcontext, RenderTargetType_COLOR_MULTISAMPLED, mAppcontext->GetWindowWidth(), mAppcontext->GetWindowHeight());

	mSenceRT.Init(mAppcontext, RenderTargetType_COLOR, mAppcontext->GetWindowWidth(), mAppcontext->GetWindowHeight());

	mBrightnessRT.Init(mAppcontext, RenderTargetType_COLOR, mAppcontext->GetWindowWidth(), mAppcontext->GetWindowHeight());

	mBluringRT.Init(mAppcontext, RenderTargetType_COLOR_BLURRING, mAppcontext->GetWindowWidth(), mAppcontext->GetWindowHeight());
}

void RenderManager::Update()
{
	ModelManager::getInstance()->Update();
}

void RenderManager::Render()
{
	Camera *mCamera = Camera::getInstance();
	mShadowRT.Enable("depthShader");
	ShaderManager::getInstance()->setMat4("lightSpaceMatrix", mCamera->lightSpaceMatrix);
	ShaderManager::getInstance()->setFloat("near_plane", mCamera->light_near);
	ShaderManager::getInstance()->setFloat("far_plane", mCamera->light_far);

	ModelManager::getInstance()->Render(RenderMode_Depth);
	GLuint depthMapRT = mShadowRT.Disable();

	mSenceRT.Enable("model");
	mSkybox->Draw(mCamera);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, depthMapRT);
	ShaderManager::getInstance()->setBool("useShadowMap", true);
	ShaderManager::getInstance()->setInt("shadowMap", 10);


	ModelManager::getInstance()->Render(RenderMode_Sence);
	GLuint SenceRT = mSenceRT.Disable();

	//mSenceRT.Render(true);

	mBrightnessRT.Enable();
	ShaderManager::getInstance()->setUseProgram("Brightness");
	mSenceRT.Render();
	GLuint brightnessRT = mBrightnessRT.Disable();
	ShaderManager::getInstance()->setUseProgram("blur");

	mBluringRT.MakeBlur(SenceRT, brightnessRT);
	//Debugging::getInstance()->DrawTex(depthMapRT, "debugShader");
}

void RenderManager::SetSkyBox(SkyBox * skybox)
{
	mSkybox = skybox;
}

RenderManager::RenderManager()
{
}


RenderManager::~RenderManager()
{
}
