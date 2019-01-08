#pragma once
#include "AppContext.h"

class GLFWContext : public AppContext
{
public:
	bool createWindow(int32_t width, int32_t height);
	ShareContext *CreateShareContext();
	bool MakeContextCurrent(ShareContext *shared_context);
	void DestroyContext();
	void SwapBuffers();
	GLFWContext();
	~GLFWContext();
};

