#pragma once
#include <cstdint>
#if defined(_WINDOWS)
#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#elif defined(ANDROID)
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif
#if defined(_WINDOWS)
typedef GLFWwindow ANativeWindow;
#endif

struct ShareContext
{
#if defined(_WINDOWS)
	ANativeWindow* window;
#elif defined(ANDROID)
	EGLSurface surface;
	EGLDisplay display;
	EGLContext shared_context;
#endif
};

class AppContext
{
protected:
	static AppContext * instance;
	ANativeWindow* window;
	int32_t width, height;
public:
	virtual bool createWindow(int32_t width, int32_t height) { return false; }
	virtual void SwapBuffers() {}
	virtual void SetWindow(ANativeWindow* window) { this->window = window; }
	virtual ShareContext CreateShareContext() { return ShareContext(); };
	virtual bool MakeContextCurrent(ShareContext shared_context) { return false; };
	virtual void DestroyContext() {};
	virtual ANativeWindow* GetWindow() { return window; }
	virtual void SetWindowSize(int32_t width, int32_t height);
	virtual int32_t GetWindowWidth() { return this->width; }
	virtual int32_t GetWindowHeight() { return this->height; }
	static AppContext * getInstance();
	AppContext(); 
	~AppContext();
};