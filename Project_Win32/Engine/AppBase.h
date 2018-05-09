#pragma once
#include <cstdint>
#include "AppContext.h"
#include "Camera.h"
#include "Logs.h"
#include "QHText.h"

class AppBase
{
protected:
	AppContext* mContext;
	Camera *mCamera;
	QHText text_FPS, text_DrawCall, text_NumTriangle;
public:
	bool initialize(int32_t width, int32_t height, ANativeWindow *window = NULL);
	void rendering();
	virtual void Draw() { return; }
	virtual void Init() { return; }
	virtual void GetRequireScreenSize(int32_t &width, int32_t &height) { return; }
	virtual void OnGameTouchEvent(int eventId, int x, int y, int pointerId);
	virtual void OnGameKeyPressed(int key, int scancode, int action, int mods) { return; };
	virtual void ZoomCamera(double xoffset, double yoffset);
	virtual AppContext* GetContext() { return mContext; }
	virtual void Resize(int width, int height);
	AppBase();
	~AppBase();
};
extern AppBase* AppFactory();
