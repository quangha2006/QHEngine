#include "FrameBuffer.h"
#include "ShaderManager.h"
#include "Debugging.h"

bool FrameBuffer::Init(AppContext * appcontext, FrameBufferType type, int texWidth, int texHeight)
{
	m_appcontext = appcontext;
	m_texBufferWidth = texWidth;
	m_texBufferHeight = texHeight;
	m_type = type;
	GLenum Status;
	glGenFramebuffers(1, &m_FBOId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId);
	// create texture
	//glGenTextures(1, &m_TexId);
	//glBindTexture(GL_TEXTURE_2D, m_TexId);
	
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	switch (type)
	{
	case FrameBufferType_DEPTH:
		LOGI("Create FrameBufferType_DEPTH: %d, %d\n", texWidth, texHeight);
		glGenTextures(1, &m_TexId);
		glBindTexture(GL_TEXTURE_2D, m_TexId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
#if defined(ANDROID)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_BORDER
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_BORDER
#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
#endif
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_texBufferWidth, m_texBufferHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		// attach depth texture as FBO's depth buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TexId, 0);
		glDrawBuffers(0, GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE)
			LOGE("ERROR!! FrameBufferType_DEPTH is not complete!\n");

		break;
	case FrameBufferType_COLORBUFFER:
		LOGI("Create FrameBufferType_COLORBUFFER: %d, %d\n", texWidth, texHeight);
		glGenTextures(1, &m_TexId);
		glBindTexture(GL_TEXTURE_2D, m_TexId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_texBufferWidth, m_texBufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexId, 0);

		// create depth buffer (renderbuffer)
		glGenRenderbuffers(1, &m_rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_texBufferWidth, m_texBufferHeight);
		// attach buffers
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE)
			LOGE("ERROR!! FrameBufferType_COLORBUFFER is not complete!\n");

		break;
	case FrameBufferType_COLORBUFFER_MULTISAMPLED:
#ifdef _WINDOWS
		LOGI("Create FrameBufferType_COLORBUFFER_MULTISAMPLED: %d, %d\n", texWidth, texHeight);
		// configure MSAA framebuffer
		glGenTextures(1, &m_TexId);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_TexId);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, m_texBufferWidth, m_texBufferHeight, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_TexId, 0);

		glGenRenderbuffers(1, &m_rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, m_texBufferWidth, m_texBufferHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE)
			LOGE("ERROR!! FrameBufferType_COLORBUFFER_MULTISAMPLED is not complete!\n");
		// configure second post-processing framebuffer

		glGenFramebuffers(1, &intermediateFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
		// create a color attachment texture

		glGenTextures(1, &screenTexture);
		glBindTexture(GL_TEXTURE_2D, screenTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_texBufferWidth, m_texBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE)
			LOGE("ERROR!! FrameBufferType_COLORBUFFER_MULTISAMPLED screenTexture is not complete!\n");
#endif
		break;
	}

	
	return true;
}

void FrameBuffer::Enable(const char* shadername)
{
	if (shadername != NULL)
		ShaderManager::getInstance()->setUseProgram(shadername);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId);
	glViewport(0, 0, m_texBufferWidth, m_texBufferHeight);
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

GLuint FrameBuffer::Disable()
{
	if (m_type == FrameBufferType_COLORBUFFER_MULTISAMPLED)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBOId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glBlitFramebuffer(0, 0, m_texBufferWidth, m_texBufferHeight, 0, 0, m_texBufferWidth, m_texBufferHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_appcontext->GetWindowWidth(), m_appcontext->GetWindowHeight());

	if (isEnableDebug)
		Debugging::getInstance()->DrawTex(m_TexId, "screenShader");
	if (m_type == FrameBufferType_COLORBUFFER_MULTISAMPLED)
		return screenTexture;
	else
		return m_TexId;
}

void FrameBuffer::EnableDebug(bool isEnable)
{
	isEnableDebug = isEnable;
}

void FrameBuffer::Render(bool useDefaultShader)
{
	if (useDefaultShader)
	{
		if (!m_default_shader.m_initialized)
			InitDefaultShader();

		m_default_shader.use();
	}
	if (quadVAO == 0) InitquadVAO();

	glBindVertexArray(quadVAO);

	glActiveTexture(GL_TEXTURE0);

	if (m_type == FrameBufferType_COLORBUFFER_MULTISAMPLED)
		glBindTexture(GL_TEXTURE_2D, screenTexture);
	else
		glBindTexture(GL_TEXTURE_2D, m_TexId);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
void FrameBuffer::InitDefaultShader()
{
	const char * verShader = {
		"#version 100\n"
		"attribute vec3 aPos;\n"
		"attribute vec2 aTexCoords;\n"
		"varying vec2 TexCoords;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	TexCoords = aTexCoords;\n"
		"	gl_Position =  vec4(aPos, 1.0);\n"
		"}\n"
	};
	const char *fragShader = {
		"#version 100\n"
		"precision highp float;\n"
		"\n"
		"varying vec2 TexCoords;\n"
		"\n"
		"uniform sampler2D tex;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = texture2D(tex, TexCoords);\n"
		"}\n"
	};

	m_default_shader.LoadShader(verShader, fragShader, true);
}

void FrameBuffer::InitquadVAO()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
}

FrameBuffer::FrameBuffer()
{
	isEnableDebug = false;
	quadVAO = 0;
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_FBOId);
	glDeleteTextures(1, &m_TexId);
}
