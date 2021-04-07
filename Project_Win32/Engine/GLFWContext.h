#pragma once
#include "AppContext.h"

class GLFWSharedContext : public AppSharedContext
{
private:
	ANativeWindow* mWindow;
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
	void SetVSync(int interval);
	GLFWContext();
	~GLFWContext();
};

