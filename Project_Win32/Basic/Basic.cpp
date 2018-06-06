#include "Basic.h"
#include "Logs.h"
#include "TextRendering.h"
#include "FrameRate.h"
#include "Timer.h"
#include "Debugging.h"
#include "QHText.h"
#include "SkyBox.h"

void Basic::Init()
{
	mCamera->Pos = glm::vec3(2.0f, 3.8f, 10.0f);
	mCamera->Target = glm::vec3(0.0f, 3.0f, 0.2f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	
	ShaderManager::getInstance()->Init("model","Shaders/model_loading.vs" ,"Shaders/model_loading.fs");
	ShaderManager::getInstance()->Init("screenShader", "Shaders/framebuffers_debug.vs", "Shaders/framebuffers_debug.fs"); // For debug
	ShaderManager::getInstance()->Init("depthShader", "Shaders/DepthShader.vs", "Shaders/DepthShader.fs");
	//ShaderManager::getInstance()->Init("model", "Shaders/BasicVS.vs", "Shaders/BasicFS.fs");
	mNanosuit.Init("Light Bulb/Light Bulb 1.dae", mCamera, false);
	mNanosuit.SetScale(glm::vec3(0.4f));
	mNanosuit.SetTranslate(glm::vec3(9.0f, 3.0f, 0.0f));
	mNanosuit.SetDrawPolygon(true);

	m_Streetenvironment.Init("Streetenvironment/Street environment_V01.obj", mCamera, false);

	//mMerce.Init("MercedesBenzSLSAMG/sls_amg.obj", mCamera, false);
	mMerce.SetRotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	mMerce.SetTranslate(glm::vec3(0.0f, 0.0f, 0.5f));

	mSun.Init("sol/sol.obj", mCamera, false, 0.0001f);
	mSun.SetUseLighting(false);
	mSun.SetCustomColor(glm::vec3(1.0f));
	//mSun.SetScale(glm::vec3(0.00001f));

	//mSpider.Init("Low-Poly Spider/Only_Spider_with_Animations_Export.obj", mCamera, true);
	//mSpider.Init("Low-Poly Spider/Spider_3.fbx", mCamera, true);
	//mSpider.Init("boblampclean/boblampclean.md5mesh", mCamera, true);
	/*mSpider.SetScale(glm::vec3(0.05f));
	mSpider.SetTranslate(glm::vec3(0.0f, 1.0f, 0.0f));
	mSpider.SetAnimPlay(0);*/

	//saberclass.Init("test/untitled.obj", mCamera, false, 3.0f);
	//saberclass.SetTranslate(glm::vec3(0.0f, 3.0f, -20.0f));
	//mGallacticCruiser.Init("GallacticCruiser/Class II Gallactic Cruiser.obj", mCamera, false, 0.1f);
	//mGallacticCruiser.SetTranslate(glm::vec3(-10.0f, -3.0f, 0.0f));

	//mMonster_1.Init("boblampclean/boblampclean.md5mesh", mCamera, false);
	//mMonster_1.SetScale(glm::vec3(0.08f));
	//mMonster_1.SetTranslate(glm::vec3(-40.0f, 0.0f, 0.0f));
	//mMonster_1.SetRotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	mframebuffer.Init(2048, 2048);
	//mframebuffer.EnableDebug(true);
	//AddText("Current Time: " + Timer::getCalendar(), 0.0f, 0.0f, 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

	mSkyBox.Init("SkyBox");
}
void Basic::Draw()
{
	mCamera->UpdateView();
	mCamera->UpdateWorldViewProjection();
	mSkyBox.Draw(mCamera);
	
	mSpider.UpdateTransform();
	/*mMonster_1.UpdateTransform();
	mMonster_1.SetTranslate(glm::vec3(0.5f, 0.0f, 0.0f));
	mMonster_1.SetRotate(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));*/

	glm::mat4 model_lamp_temp;
	glm::vec3 lampPos = glm::vec3(8.2f, 10.0f, 9.0f);;
	//float timestamp_for_lamp = Timer::getMillisecond()/1000.0f;
	model_lamp_temp = glm::rotate(model_lamp_temp, glm::radians(timestamp_for_lamp * 10), glm::vec3(0.0f, 1.0f, 0.0f));
	model_lamp_temp = glm::translate(model_lamp_temp, lampPos);
	model_lamp_temp = glm::scale(model_lamp_temp, glm::vec3(0.4f));

	glm::vec3 lamppos;
	lamppos.x = model_lamp_temp[3].x;
	lamppos.y = model_lamp_temp[3].y;
	lamppos.z = model_lamp_temp[3].z;
	//glm::mat4 lookat = (mCamera->projection) * (mCamera->view); // need to * with model in model.draw
	
	mframebuffer.Enable(mContext->GetWindowWidth(), mContext->GetWindowHeight());
	
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.1f, far_plane = 40.0f;
	lightProjection = glm::ortho(-12.0f, 15.0f, -5.0f, 14.0f, near_plane, far_plane);
	//lightProjection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
	lightView = glm::lookAt(lamppos, glm::vec3(0.0f), glm::vec3(0.0, 3.0, 0.2));
	lightSpaceMatrix = lightProjection * lightView;

	ShaderManager::getInstance()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	ShaderManager::getInstance()->setFloat("near_plane", near_plane);
	ShaderManager::getInstance()->setFloat("far_plane", far_plane);

	mNanosuit.Draw(lamppos);	
	mMerce.Draw(lamppos);
	mSpider.Draw(lamppos);
	saberclass.Draw(lamppos);
	mGallacticCruiser.Draw(lamppos);
	
	GLuint depthMap = mframebuffer.Disable();
	
	ShaderManager::getInstance()->setUseProgram("model");
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	ShaderManager::getInstance()->setInt("shadowMap",10);
	ShaderManager::getInstance()->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	m_Streetenvironment.Draw(lamppos);

	mNanosuit.Draw(lamppos);

	mSun.SetWorld();
	mSun.SetTranslate(lampPos);
	mSun.Draw(lamppos);

	mMerce.Draw(lamppos);
	mSpider.Draw(lamppos);
	saberclass.Draw(lamppos);
	mGallacticCruiser.Draw(lamppos);

	mMonster_1.Draw(lamppos);
}
void Basic::GetRequireScreenSize(int32_t &width, int32_t &height)
{
	width = 900;
	height = 540;
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
	Pos_model_Original = glm::translate(Pos_model_Original, glm::vec3(0.0f, 0.0f, 0.0f));
	timestamp_for_lamp = 0;
}

Basic::~Basic()
{
}
AppBase* AppFactory()
{
	return new Basic();
}