#include "Basic.h"
#include "Logs.h"
#include "TextRendering.h"
#include "FrameRate.h"
#include "Timer.h"
#include "Debugging.h"
#include "QHText.h"

void Basic::Init()
{
	mCamera->Pos = glm::vec3(2.0f, 3.8f, 10.0f);
	mCamera->Target = glm::vec3(0.0f, 3.0f, 0.2f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	
	ShaderManager::getInstance()->Init("model","Shaders/model_loading.vs" ,"Shaders/model_loading.fs");
	//ShaderManager::getInstance()->Init("screenShader", "Shaders/framebuffers_screen.vs", "Shaders/framebuffers_screen.fs"); // For debug
	ShaderManager::getInstance()->Init("depthShader", "Shaders/DepthShader.vs", "Shaders/DepthShader.fs");
	//ShaderManager::getInstance()->Init("model", "Shaders/BasicVS.vs", "Shaders/BasicFS.fs");
	//mNanosuit.Init("nanosuit/nanosuit.obj", mCamera, false);
	//m_Streetenvironment.Init("Streetenvironment/Street environment_V01.obj", mCamera, false);
	mMerce.Init("MercedesBenzSLSAMG/sls_amg.obj", mCamera, false);
	//mSun.Init("sol/sol.obj", mCamera, false, 0.0001f);
	//mSun.SetUseLighting(false);
	//mSun.SetCustomColor(glm::vec3(1.0f));

	//mSpider.Init("test/model.obj", mCamera, true);
	//mSpider.Init("boblampclean/boblampclean.md5mesh", mCamera, true);
	//saberclass.Init("test/untitled.obj", mCamera, false, 3.0f);
	//mGallacticCruiser.Init("GallacticCruiser/Class II Gallactic Cruiser.obj", mCamera, false, 0.1f);

	//mMonster_1.Init("Monster_1/Monster_1.dae", mCamera, false);

	mframebuffer.Init(2048, 2048);
	//AddText("Current Time: " + Timer::getCalendar(), 0.0f, 0.0f, 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
}
void Basic::Draw()
{
	glm::mat4 model_lamp_temp;
	glm::vec3 lampPos = glm::vec3(10.2f, 6.0f, 9.0f);;
	float timestamp_for_lamp = Timer::getMillisecond()/1000.0f;
	model_lamp_temp = glm::rotate(model_lamp_temp, glm::radians(timestamp_for_lamp * 10), glm::vec3(0.0f, 1.0f, 0.0f));
	model_lamp_temp = glm::translate(model_lamp_temp, lampPos);
	model_lamp_temp = glm::scale(model_lamp_temp, glm::vec3(0.4f));

	glm::vec3 lamppos;
	lamppos.x = model_lamp_temp[3].x;
	lamppos.y = model_lamp_temp[3].y;
	lamppos.z = model_lamp_temp[3].z;
	glm::mat4 lookat = (mCamera->projection) * (mCamera->view); // need to * with model in model.draw
	
	//mframebuffer.Enable(mContext->GetWindowWidth(), mContext->GetWindowHeight());
	//
	//glm::mat4 lightProjection, lightView;
	//glm::mat4 lightSpaceMatrix;
	//float near_plane = 0.1f, far_plane = 50.0f;
	//lightProjection = glm::ortho(-12.0f, 15.0f, -4.0f, 14.0f, near_plane, far_plane);
	////lightProjection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
	//lightView = glm::lookAt(lamppos, glm::vec3(0.0f), glm::vec3(0.0, 3.0, 0.2));
	//lightSpaceMatrix = lightProjection * lightView;

	//ShaderManager::getInstance()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	//ShaderManager::getInstance()->setFloat("near_plane", near_plane);
	//ShaderManager::getInstance()->setFloat("far_plane", far_plane);

	//mNanosuit.Draw(glm::translate(glm::scale(Pos_model_Original,glm::vec3(0.7f)), glm::vec3(9.0f, 0.0f, 0.0f)), lookat, lamppos);	
	//mMerce.Draw(glm::translate(glm::rotate(Pos_model_Original, 11.0f, glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.0f, 1.0f, 2.0f)), lookat, lamppos);
	//mSpider.Draw(glm::rotate(glm::translate(glm::scale(Pos_model_Original, glm::vec3(0.05f)), glm::vec3(-2.0f, -0.2f, 0.0f)), glm::radians(timestamp_for_lamp * -1), glm::vec3(0.0f, 1.0f, 0.0f)), lookat, lamppos);
	//saberclass.Draw(glm::translate(Pos_model_Original, glm::vec3(0.0f, 3.0f, -20.0f)), lookat, lamppos);
	//mGallacticCruiser.Draw(glm::translate(Pos_model_Original, glm::vec3(-10.0f, -3.0f, 0.0f)), lookat, lamppos);
	//
	//GLuint depthMap = mframebuffer.Disable();
	//
	ShaderManager::getInstance()->setUseProgram("model");
	//glActiveTexture(GL_TEXTURE10);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//ShaderManager::getInstance()->setInt("shadowMap",10);
	//ShaderManager::getInstance()->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	m_Streetenvironment.Draw(glm::translate(Pos_model_Original, glm::vec3(0.0f, 0.0f, 0.0f)), lookat, lamppos);

	mNanosuit.Draw(glm::translate(glm::scale(Pos_model_Original, glm::vec3(0.7f)), glm::vec3(9.0f, 0.0f, 0.0f)), lookat, lamppos);
	mSun.Draw(glm::scale(glm::translate(Pos_model_Original, lamppos), glm::vec3(0.0001f)), lookat, lamppos);
	mMerce.Draw(glm::translate(glm::rotate(Pos_model_Original, 11.0f, glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.0f, 0.0f, 0.51f)), lookat, lamppos);
	mSpider.Draw(glm::rotate(glm::translate(glm::scale(Pos_model_Original, glm::vec3(0.05f)), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(timestamp_for_lamp * -1), glm::vec3(0.0f, 1.0f, 0.0f)), lookat, lamppos);
	saberclass.Draw(glm::translate(glm::scale(Pos_model_Original, glm::vec3(5.0f)), glm::vec3(0.0f, 1.0f, 0.0f)), lookat, lamppos);
	mGallacticCruiser.Draw(glm::translate(Pos_model_Original, glm::vec3(-10.0f, -3.0f, 0.0f)), lookat, lamppos);


	mMonster_1.Draw(glm::translate(Pos_model_Original, glm::vec3(0.0f, 0.0f, 0.0f)), lookat, lamppos);
}
void Basic::GetRequireScreenSize(int32_t &width, int32_t &height)
{
	width = 900;
	height = 540;
}
Basic::Basic()
{
	Pos_model_Original = glm::translate(Pos_model_Original, glm::vec3(0.0f, 0.0f, 0.0f));
}

Basic::~Basic()
{
}
AppBase* AppFactory()
{
	return new Basic();
}