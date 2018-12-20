#pragma once
#include "stdafx.h"
#include <AppContext.h>

enum FrameBufferType
{
	FrameBufferType_DEPTH,
	FrameBufferType_HDRCOLOR
};

class FrameBuffer
{
private:
	GLuint depthMapFBO;
	GLuint m_texBufferWidth, m_texBufferHeight;
	GLuint texdepthMap;
	bool isEnableDebug;
	AppContext * m_appcontext;
public:
	bool Init(AppContext * appcontext, FrameBufferType type, int texBufferWidth = 1024, int texBufferHeight = 1024);
	void Enable(const char* shadername);
	GLuint Disable();
	void EnableDebug(bool isEnable);
	FrameBuffer();
	~FrameBuffer();
};