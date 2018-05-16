#pragma once
#include "stdafx.h"
class FrameBuffer
{
private:
	GLuint depthMapFBO;
	GLuint width, height;
	int32_t current_screen_width, current_screen_height;
	GLuint texdepthMap;
	bool isEnableDebug;
public:
	bool Init(int texWidth = 1024, int texHeight = 1024);
	void Enable(int32_t screen_width, int32_t screen_height);
	GLuint Disable();
	void EnableDebug(bool isEnable);
	FrameBuffer();
	~FrameBuffer();
};