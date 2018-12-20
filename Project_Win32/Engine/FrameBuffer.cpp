#include "FrameBuffer.h"
#include "ShaderManager.h"
#include "Debugging.h"

bool FrameBuffer::Init(AppContext * appcontext, FrameBufferType type, int texWidth, int texHeight)
{
	m_appcontext = appcontext;
	m_texBufferWidth = texWidth;
	m_texBufferHeight = texHeight;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	// create depth texture
	glGenTextures(1, &texdepthMap);
	glBindTexture(GL_TEXTURE_2D, texdepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_texBufferWidth, m_texBufferHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
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

void FrameBuffer::Enable(const char* shadername)
{
	ShaderManager::getInstance()->setUseProgram(shadername);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glViewport(0, 0, m_texBufferWidth, m_texBufferHeight);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
}

GLuint FrameBuffer::Disable()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_appcontext->GetWindowWidth(), m_appcontext->GetWindowHeight());
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
	isEnableDebug = false;
}


FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &texdepthMap);
}
