#include "AppBase.h"
#include "FrameRate.h"
#include "Debugging.h"
#include "TextRendering.h"
#include "Utils.h"
#include "QHAxis.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#if defined(_WINDOWS)
#include "GLFWContext.h"
#elif defined(ANDROID)
#include "EGLAppContext.h"
#include <time.h>
#include <unistd.h>
#endif
#include <vector>
#include <stdlib.h>

bool AppBase::initialize(int32_t width, int32_t height, ANativeWindow *window)
{
#if defined(_WINDOWS)
	mContext = new GLFWContext();
#elif defined(ANDROID)
	mContext = new EGLAppContext();
#endif
	mContext->createWindow(width, height);
	mCamera = new Camera();
	mCamera->projection = glm::perspective(glm::radians(mCamera->zoom), (float)(width) / (float)(height), mCamera->View_near, mCamera->View_far);
	FrameRate::getInstance();
	//FrameRate::getInstance()->setLimitFPS(30);

	TextRendering::getInstance()->Init("fonts/VBAMASH.TTF", width, height);
	Init();

	text_FPS.setPos(0, 0);
	text_FPS.setScale(0.5f);
	text_FPS.setColor(glm::vec3(0.0f, 1.0f, 0.0f));
	text_FPS.setText("FPS: 60.0");

	text_DrawCall.setPos(text_FPS.getEndPos());
	text_DrawCall.setScale(0.5f);
	text_DrawCall.setColor(glm::vec3(0.0f, 1.0f, 0.0f));

	text_NumTriangle.setPos(265,0);
	text_NumTriangle.setScale(0.5f);
	text_NumTriangle.setColor(glm::vec3(0.0f, 1.0f, 0.0f));

	text_FrameTime.setPos(0, 18);
	text_FrameTime.setScale(0.25f);
	text_FrameTime.setColor(glm::vec3(0.0f, 1.0f, 0.0f));

	return true;
}

void AppBase::rendering()
{
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	mCamera->UpdateWorldViewProjection();
	Draw();
	unsigned short numDrawCall = Debugging::getInstance()->getNumDrawCall();
	int numTriangle = Debugging::getInstance()->getNumTriangle();
	int numdrawcall = Debugging::getInstance()->getNumDrawCall();
	float fps = FrameRate::getInstance()->GetFPS();
	int frameTime = FrameRate::getInstance()->GetPrevFrameTime();

	std::string tmp_fps = Utils::toString("FPS: %.1f", fps);
	std::string tmp_drawcall = Utils::toString("DrawCall: %d", numdrawcall);
	std::string tmp_numtriangle = Utils::toString("numTriangle: %d", numTriangle);
	std::string tmp_PrevFraTime = Utils::toString("PrevFrameTime: %d", frameTime);

	text_FPS.setText(tmp_fps);
	text_DrawCall.setPos(text_FPS.getEndPos());
	text_DrawCall.setText(tmp_drawcall);
	text_NumTriangle.setText(tmp_numtriangle);
	text_FrameTime.setText(tmp_PrevFraTime);

	FrameRate::getInstance()->Counter();
	Debugging::getInstance()->resetCount();
	TextRendering::getInstance()->Draw();
}

void AppBase::Resize(int width, int height)
{
	LOGI("set glViewport: %d, %d", width, height);
	glViewport(0, 0, width, height);
	mContext->SetWindowSize(width, height);
	mCamera->projection = glm::perspective(glm::radians(mCamera->zoom), (float)(width) / (float)(height), mCamera->View_near, mCamera->View_far);
	TextRendering::getInstance()->UpdateScreenSize(width, height);
}
void AppBase::OnGameTouchEvent(int eventId, int x, int y, int pointerId)
{
	static int touch_old_x = 0;
	static int touch_old_y = 0;
	static bool preesed = false;
	if (eventId == 0)
	{
		touch_old_x = x;
		touch_old_y = y;
		preesed = true;
		return;
	}
	if (eventId == 2)
	{
		touch_old_x = 0;
		touch_old_y = 0;
		preesed = false;
		return;
	}
	if (!preesed) return;
	
	mCamera->view = glm::rotate(mCamera->view, glm::radians((float)(x - touch_old_x)), glm::vec3(0.0f, 1.0f, 0.0f));

	//mCamera->view = glm::rotate(mCamera->view, glm::radians((float)(y - touch_old_y)), glm::vec3(1.0f, 0.0f, 1.0f));
	//mCamera->view = glm::rotate(mCamera->view, glm::radians((float)(y - touch_old_y)), glm::vec3(0.0f, 0.0f, 1.0f));

	//mCamera->view = glm::rotate(mCamera->view, glm::radians((float)(y - touch_old_y)), glm::vec3(0.0f, mCamera->Pos.y, mCamera->Pos.z));
	//glm::mat4::
	mCamera->Pos = mCamera->ExtractCameraPos(mCamera->view);

	mCamera->Pos.y += ((y - touch_old_y)*0.1);

	//if ((mCamera->Pos.y > 0.5) && (mCamera->Pos.y < 80))
	//{
	//	if (mCamera->Pos.x > 0)
	//		mCamera->Pos.x -= ((y - touch_old_y)*0.3);
	//	else
	//		mCamera->Pos.x += ((y - touch_old_y)*0.3);
	//	if (mCamera->Pos.z > 0)
	//		mCamera->Pos.z -= ((y - touch_old_y)*0.3);
	//	else
	//		mCamera->Pos.z += ((y - touch_old_y)*0.3);
	//}

	if (mCamera->Pos.y > 60) mCamera->Pos.y = 60;
	if (mCamera->Pos.y < 0.5) mCamera->Pos.y = 0.5;

	touch_old_x = x;
	touch_old_y = y;
}
void AppBase::ZoomCamera(double xoffset, double yoffset)
{
	int offset = 20;
	float dis_x = (mCamera->Pos.x - mCamera->Target.x) / offset;
	float dis_y = (mCamera->Pos.y - mCamera->Target.y) / offset;
	float dis_z = (mCamera->Pos.z - mCamera->Target.z) / offset;

	if (yoffset > 0.0f)
	{
		mCamera->Pos.x -= dis_x;
		mCamera->Pos.y -= dis_y;
		mCamera->Pos.z -= dis_z;

		mCamera->Target.x -= dis_x;
		mCamera->Target.y -= dis_y;
		mCamera->Target.z -= dis_z;
	}
	else
	{
		mCamera->Pos.x += dis_x;
		mCamera->Pos.y += dis_y;
		mCamera->Pos.z += dis_z;

		mCamera->Target.x += dis_x;
		mCamera->Target.y += dis_y;
		mCamera->Target.z += dis_z;
	}
}
AppBase::AppBase()
{
}

AppBase::~AppBase()
{
	delete mContext;
}
