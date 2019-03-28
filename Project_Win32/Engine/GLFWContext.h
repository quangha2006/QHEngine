#pragma once
#include "AppContext.h"

class GLFWSharedContext : public AppSharedContext
{
private:
	ANativeWindow* window;
public:
	void SetWindows(ANativeWindow* window);
	bool MakeContextCurrent();
	void DestroyContext();
};

class GLFWContext : public AppContext
{
public:
	bool createWindow(int32_t width, int32_t height);
	AppSharedContext *CreateShareContext();
	void DestroyContext();
	void SwapBuffers();
	void SwapInterval(int interval);
	GLFWContext();
	~GLFWContext();
};

