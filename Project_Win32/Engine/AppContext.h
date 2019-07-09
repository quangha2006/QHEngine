#ifndef __APPCONTEXT_H__
#define __APPCONTEXT_H__
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
typedef void *EGLContext;
typedef void *EGLDisplay;
typedef void *EGLSurface;
#endif

class AppSharedContext
{
public:
	virtual bool MakeContextCurrent() { return false; };
	virtual void DestroyContext() {};
	virtual void SetWindows(ANativeWindow* window) {};
	virtual void SetSurface(EGLSurface surface) {};
	virtual void SetDisplay(EGLDisplay display) {};
	virtual void SetContext(EGLContext shared_context) {};
};

class AppContext
{
protected:
	static AppContext * instance;
	ANativeWindow* window;
	int32_t width, height;
public:
	virtual bool createWindow(int32_t width, int32_t height) { return false; }
	virtual void SwapInterval(int interval) {}
	virtual void SwapBuffers() {}
	virtual void SetWindow(ANativeWindow* window) { this->window = window; }
	virtual AppSharedContext *CreateShareContext() { return new AppSharedContext(); };
	virtual void DestroyContext() {};
	virtual ANativeWindow* GetWindow() { return window; }
	virtual void SetWindowSize(int32_t width, int32_t height);
	virtual int32_t GetWindowWidth() { return this->width; }
	virtual int32_t GetWindowHeight() { return this->height; }
	static AppContext * getInstance();
	AppContext(); 
	~AppContext();
};
#endif //__APPCONTEXT_H__