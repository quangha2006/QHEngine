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
	EGLDisplay mDisplay;
	EGLSurface mSurface;
	bool Initializee;
public:
	bool createWindow(int32_t width, int32_t height);
	void SwapBuffers();
	EGLAppContext();
	~EGLAppContext();
};