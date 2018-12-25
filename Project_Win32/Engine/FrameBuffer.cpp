#include "FrameBuffer.h"
#include "ShaderManager.h"
#include "Debugging.h"

bool FrameBuffer::Init(AppContext * appcontext, FrameBufferType type, int texWidth, int texHeight)
{
	m_appcontext = appcontext;
	m_texBufferWidth = texWidth;
	m_texBufferHeight = texHeight;
	glGenFramebuffers(1, &m_FBOId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId);
	// create depth texture
	glGenTextures(1, &m_TexId);
	glBindTexture(GL_TEXTURE_2D, m_TexId);
	
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	switch (type)
	{
	case FrameBufferType_DEPTH:
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
		break;
	case FrameBufferType_HDRCOLOR:
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
		break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
		LOGE("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");

	// Disable writes to the color buffer
	//glDrawBuffers(0, GL_NONE);
	//glReadBuffer(GL_NONE);

	return true;
}

void FrameBuffer::Enable(const char* shadername)
{
	if (shadername != NULL)
		ShaderManager::getInstance()->setUseProgram(shadername);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId);
	glViewport(0, 0, m_texBufferWidth, m_texBufferHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
}

GLuint FrameBuffer::Disable()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_appcontext->GetWindowWidth(), m_appcontext->GetWindowHeight());
	//glDisable(GL_CULL_FACE);
	if (isEnableDebug)
		Debugging::getInstance()->DrawTex(m_TexId, "basic");
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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TexId);
	glBindVertexArray(quadVAO);
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
