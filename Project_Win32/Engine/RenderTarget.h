#pragma once
#include "stdafx.h"
#include <AppContext.h>
#include <Shader.h>

enum RenderTargetType
{
	RenderTargetType_DEPTH,
	RenderTargetType_COLOR,
	RenderTargetType_COLOR_MULTISAMPLED,
	RenderTargetType_COLOR_BRIGHTNESS,
	RenderTargetType_COLOR_BLURRING
};

class RenderTarget
{
private:
	GLuint m_FBOId[2], intermediateFBO;
	GLuint m_rboDepth;
	GLuint m_texBufferWidth, m_texBufferHeight;
	GLuint m_TexId[2], screenTexture;

	bool isEnableDebug;
	RenderTargetType m_type;
	AppContext * m_appcontext;
	Shader m_default_shader;
	void InitDefaultShader();
	void InitquadVAO();
	GLuint quadVAO;
	GLuint quadVBO;
public:
	bool Init(AppContext * appcontext, RenderTargetType type, int texBufferWidth = 1024, int texBufferHeight = 1024);
	void Enable(const char* shadername = NULL);
	GLuint Disable();
	int GetTextureId(int index);
	void EnableDebug(bool isEnable);
	void Render(bool useDefaultShader = false);
	void MakeBloom(GLuint normalTexture, GLuint BriTexture);
	RenderTarget();
	~RenderTarget();
};
