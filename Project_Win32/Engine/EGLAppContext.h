#pragma once
#include "AppContext.h"
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#if defined(ANDROID)
#include <android/sensor.h>
#endif

class EGLSharedContext : public AppSharedContext
{
private:
	EGLSurface surface;
	EGLDisplay display;
	EGLContext shared_context;
public:
	bool MakeContextCurrent();
	void DestroyContext();
	void SetSurface(EGLSurface surface);
	void SetDisplay(EGLDisplay display);
	void SetContext(EGLContext shared_context);
};

class EGLAppContext : public AppContext
{
private:
	EGLDisplay display;
	bool Initializee;
public:
	bool createWindow(int32_t width, int32_t height);
	AppSharedContext *CreateShareContext();
	void DestroyContext();
	void SwapBuffers();
	EGLAppContext();
	~EGLAppContext();
};