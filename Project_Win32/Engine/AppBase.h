#pragma once
#include <cstdint>
#include "AppContext.h"
#include "Camera.h"
#include "Logs.h"
#include "QHText.h"
#include "QHAxis.h"
#include "SkyBox.h"
#include "Timer.h"
class AppBase
{
protected:
	AppContext* mContext;
	Camera *mCamera;
	QHText text_FPS, text_DrawCall, text_NumTriangle, text_FrameTime;
	QHAxis axis;
	SkyBox mSkyBox;
	bool mIsLoadingThreadFinish;
public:
	bool initialize(int32_t width, int32_t height, ANativeWindow *window = NULL);
	void rendering();
	void LoadingThread(AppSharedContext *shared_context);
	void GameTouchEvent(int eventId, int x, int y, int pointerId);
	void GameKeyPressed(int key, int scancode, int action, int mods);
	void GameZoomCamera(double xoffset, double yoffset);
	virtual void Update(int delta) {}
	virtual void Init() {}
	virtual void GetRequireScreenSize(int32_t &width, int32_t &height) {}
	virtual bool OnGameTouchEvent(int eventId, int x, int y, int pointerId) { return false; };
	virtual bool OnGameKeyPressed(int key, int scancode, int action, int mods) { return false; };
	virtual bool OnGameZoomCamera(double xoffset, double yoffset) { return false; };
	virtual AppContext* GetContext() { return mContext; }
	virtual void Resize(int width, int height);
	virtual void OnGameLoadingThreadFinished(int loadingtimeinms) {};
	AppBase();
	~AppBase();
};
extern AppBase* AppFactory();
