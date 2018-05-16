#include "FrameBuffer.h"
#include "ShaderManager.h"
#include "Debugging.h"

bool FrameBuffer::Init(int texWidth, int texHeight)
{
	width = texWidth;
	height = texHeight;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	// create depth texture
	glGenTextures(1, &texdepthMap);
	glBindTexture(GL_TEXTURE_2D, texdepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
#if defined(ANDROID)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_BORDER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_BORDER
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
#endif
	//glGenerateMipmap(GL_TEXTURE_2D);
	// attach depth texture as FBO's depth buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texdepthMap, 0);

	// Disable writes to the color buffer
	glDrawBuffers(0,GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
		LOGE("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void FrameBuffer::Enable(int32_t screen_width, int32_t screen_height)
{
	current_screen_width = screen_width;
	current_screen_height = screen_height;

	ShaderManager::getInstance()->setUseProgram("depthShader");

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
}

GLuint FrameBuffer::Disable()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, current_screen_width, current_screen_height);
	//glDisable(GL_CULL_FACE);
	if (isEnableDebug)
		Debugging::getInstance()->DrawTex(texdepthMap);
	return texdepthMap;
}

void FrameBuffer::EnableDebug(bool isEnable)
{
	isEnableDebug = isEnable;
}

FrameBuffer::FrameBuffer()
{
	width = 1024;
	height = 1024;
	isEnableDebug = false;
}


FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &texdepthMap);
}
