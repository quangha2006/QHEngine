#include "EnvMapping.h"
#include <Debugging.h>

void EnvMapping::Draw()
{
	mSkyBox.Draw(mCamera);
	glm::vec3 lampPos = glm::vec3(8.2f, 10.0f, 9.0f);

	mframebuffer.Enable(mContext->GetWindowWidth(), mContext->GetWindowHeight());

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.1f, far_plane = 40.0f;
	lightProjection = glm::ortho(-12.0f, 15.0f, -5.0f, 14.0f, near_plane, far_plane);
	lightView = glm::lookAt(lampPos, glm::vec3(0.0f), glm::vec3(0.0, 3.0, 0.2));
	lightSpaceMatrix = lightProjection * lightView;

	ShaderManager::getInstance()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	ShaderManager::getInstance()->setFloat("near_plane", near_plane);
	ShaderManager::getInstance()->setFloat("far_plane", far_plane);

	mNanosuit.Draw(lampPos);

	GLuint depthMap = mframebuffer.Disable();

	ShaderManager::getInstance()->setUseProgram("model");
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	ShaderManager::getInstance()->setInt("shadowMap", 10);
	ShaderManager::getInstance()->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	mNanosuit.Draw(lampPos);

	//Debugging::getInstance()->DrawTex(depthMap);
}

void EnvMapping::Init()
{
	mCamera->Pos = glm::vec3(3.0f, 3.0f, 3.0f);
	mCamera->Target = glm::vec3(0.0f, 2.0f, 0.2f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);

	ShaderManager::getInstance()->Init("model", "Shaders/Model_Environment_mapping.vs", "Shaders/Model_Environment_mapping.fs");
	ShaderManager::getInstance()->Init("screenShader", "Shaders/framebuffers_debug.vs", "Shaders/framebuffers_debug.fs"); // For debug
	ShaderManager::getInstance()->Init("depthShader", "Shaders/DepthShader.vs", "Shaders/DepthShader.fs");

	mSkyBox.Init("SkyBox");

	mNanosuit.Init("nanosuit/nanosuit.obj", mCamera, false);
	mNanosuit.SetScale(glm::vec3(0.2f));
	mNanosuit.SetTranslate(glm::vec3(0.0f, 0.0f, 0.0f));

	mframebuffer.Init(2048, 2048);


}

void EnvMapping::GetRequireScreenSize(int32_t & width, int32_t & height)
{
	width = 900;
	height = 540;
}

void EnvMapping::OnGameKeyPressed(int key, int scancode, int action, int mods)
{
}

EnvMapping::EnvMapping()
{
}


EnvMapping::~EnvMapping()
{
}

AppBase* AppFactory()
{
	return new EnvMapping();
}