#pragma once
#include "stdafx.h"
#include <AppContext.h>
#include <Shader.h>

enum FrameBufferType
{
	FrameBufferType_DEPTH,
	FrameBufferType_COLORBUFFER,
	FrameBufferType_COLORBUFFER_MULTISAMPLED,
	FrameBufferType_COLORBUFFER_BRIGHTNESS,
	FrameBufferType_COLORBUFFER_BLURRING
};

class FrameBuffer
{
private:
	GLuint m_FBOId[2], intermediateFBO;
	GLuint m_rboDepth;
	GLuint m_texBufferWidth, m_texBufferHeight;
	GLuint m_TexId[2], screenTexture;

	bool isEnableDebug;
	FrameBufferType m_type;
	AppContext * m_appcontext;
	Shader m_default_shader;
	void InitDefaultShader();
	void InitquadVAO();
	GLuint quadVAO;
	GLuint quadVBO;
public:
	bool Init(AppContext * appcontext, FrameBufferType type, int texBufferWidth = 1024, int texBufferHeight = 1024);
	void Enable(const char* shadername = NULL);
	GLuint Disable();
	int GetTextureId(int index);
	void EnableDebug(bool isEnable);
	void Render(bool useDefaultShader = false);
	void MakeBlur(GLuint normalTexture,GLuint BriTexture);
	FrameBuffer();
	~FrameBuffer();
};