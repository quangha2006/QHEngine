#include "EGLAppContext.h"
#include "Logs.h"
#include <thread>
#include <time.h>
#include <unistd.h>

bool EGLAppContext::createWindow(int32_t width, int32_t height)
{
	this->width = width;
	this->height = height;

	Initializee = true;
	return true;
}

void EGLAppContext::SwapInterval(int interval)
{
	eglSwapInterval(eglGetCurrentDisplay(), interval);
}

AppSharedContext *EGLAppContext::CreateShareContext()
{
	AppSharedContext *shared_context = new EGLSharedContext();

	EGLContext context = eglGetCurrentContext();
	EGLDisplay display = eglGetCurrentDisplay();
	EGLConfig config;
	EGLint numConfigs;
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
	EGLint attribList[] =
	{
		EGL_LEVEL, 0,
		EGL_RENDERABLE_TYPE, 4,
		EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
		EGL_RED_SIZE,       8,
		EGL_GREEN_SIZE,     8,
		EGL_BLUE_SIZE,      8,
		EGL_ALPHA_SIZE,     8,
		EGL_DEPTH_SIZE,     24,
		EGL_SAMPLE_BUFFERS, 1,
		EGL_SAMPLES, 4,
		EGL_NONE
	};
	eglChooseConfig(display, attribList, &config, 1, &numConfigs);
	EGLContext sharecontext = eglCreateContext(display, config, context, contextAttribs);
	EGLSurface surface = eglCreatePbufferSurface(display, config, attribList);

	if (sharecontext == EGL_NO_CONTEXT)
		LOGE("ERROR! EGL_NO_CONTEXT\n");
	if (display == EGL_NO_DISPLAY)
		LOGI("ERROR! EGL_NO_DISPLAY\n");

	shared_context->SetDisplay(display);
	shared_context->SetSurface(surface);
	shared_context->SetContext(sharecontext);
	this->display = display;
	return shared_context;
}

void EGLAppContext::DestroyContext()
{
	eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void EGLAppContext::SwapBuffers()
{
	if (Initializee) {
		//eglSwapBuffers(mDisplay, mSurface);
	}
}

EGLAppContext::EGLAppContext()
{
	Initializee = false;
	this->instance = this;
}

EGLAppContext::~EGLAppContext()
{
}

bool EGLSharedContext::MakeContextCurrent()
{
	return eglMakeCurrent(display, surface, surface, shared_context);
}

void EGLSharedContext::DestroyContext()
{
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void EGLSharedContext::SetSurface(EGLSurface surface)
{
	this->surface = surface;
}

void EGLSharedContext::SetDisplay(EGLDisplay display)
{
	this->display = display;
}

void EGLSharedContext::SetContext(EGLContext shared_context)
{
	this->shared_context = shared_context;
}
