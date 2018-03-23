#pragma once
#include "AppContext.h"

class GLFWContext : public AppContext
{
private:

public:
	bool createWindow(int32_t width, int32_t height);
	void SwapBuffers();
	GLFWContext();
	~GLFWContext();
};

