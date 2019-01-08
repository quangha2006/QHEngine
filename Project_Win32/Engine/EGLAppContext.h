#pragma once
#include "AppContext.h"
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#if defined(ANDROID)
#include <android/sensor.h>
#endif

class EGLAppContext : public AppContext
{
private:
	EGLDisplay display;
	bool Initializee;
public:
	bool createWindow(int32_t width, int32_t height);
	ShareContext *CreateShareContext();
	bool MakeContextCurrent(ShareContext *shared_context);
	void DestroyContext();
	void SwapBuffers();
	EGLAppContext();
	~EGLAppContext();
};