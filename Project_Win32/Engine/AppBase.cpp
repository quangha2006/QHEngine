#include "AppBase.h"
#include "FrameRate.h"
#include "Debugging.h"
#include "TextRendering.h"
#include "Utils.h"
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
	//FrameRate::getInstance()->setLimitFPS(60);

	TextRendering::getInstance()->Init("fonts/VBAMASH.TTF", width, height);
	Init();

	text_FPS.setPos(0, 0);
	text_DrawCall.setPos(100,0);
	text_NumTriangle.setPos(265,0);
	
	text_FPS.setScale(0.5f);			text_FPS.setColor(glm::vec3(0.0f, 1.0f, 0.0f));
	text_DrawCall.setScale(0.5f);		text_DrawCall.setColor(glm::vec3(0.0f, 1.0f, 0.0f));
	text_NumTriangle.setScale(0.5f);	text_NumTriangle.setColor(glm::vec3(0.0f, 1.0f, 0.0f));

	return true;
}

void AppBase::rendering()
{
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	Draw();
	unsigned short numDrawCall = Debugging::getInstance()->getNumDrawCall();
	int numTriangle = Debugging::getInstance()->getNumTriangle();
	int numdrawcall = Debugging::getInstance()->getNumDrawCall();
	float fps = FrameRate::getInstance()->GetFPS();

	std::string tmp_fps = Utils::toString("FPS: %.1f", fps);
	std::string tmp_drawcall = Utils::toString("DrawCall: %d", numdrawcall);
	std::string tmp_numtriangle = Utils::toString("numTriangle: %d", numTriangle);

	text_DrawCall.setText(tmp_drawcall);
	text_NumTriangle.setText(tmp_numtriangle);
	text_FPS.setText(tmp_fps);

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
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	touch_old_x = x;
	touch_old_y = y;
}
void AppBase::ZoomCamera(double xoffset, double yoffset)
{
	if (mCamera->zoom >= 1.0f && mCamera->zoom <= 45.0f)
		mCamera->zoom -= (float)yoffset;
	if (mCamera->zoom <= 1.0f)
		mCamera->zoom = 1.0f;
	if (mCamera->zoom >= 45.0f)
		mCamera->zoom = 45.0f;
	mCamera->projection = glm::perspective(glm::radians(mCamera->zoom), (float)(mContext->GetWindowWidth()) / (float)(mContext->GetWindowHeight()), mCamera->View_near, mCamera->View_far);
}
AppBase::AppBase()
{
}

AppBase::~AppBase()
{
	delete mContext;
}
