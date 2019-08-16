#include "RenderTarget.h"
#include "ShaderManager.h"
#include "Debugging.h"

bool RenderTarget::Init(AppContext * appcontext, RenderTargetType type, int texWidth, int texHeight)
{
	m_appcontext = appcontext;
	m_texBufferWidth = texWidth;
	m_texBufferHeight = texHeight;
	m_type = type;
	GLenum Status;
	glGenFramebuffers(1, &m_FBOId[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId[0]);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	switch (type)
	{
	case RenderTargetType_DEPTH:
		LOGI("Create RenderTargetType_DEPTH: %d, %d\n", texWidth, texHeight);
		glGenFramebuffers(1, &m_FBOId[0]);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId[0]);
		glGenTextures(1, &m_TexId[0]);
		glBindTexture(GL_TEXTURE_2D, m_TexId[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
#if defined(ANDROID)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_BORDER
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //GL_CLAMP_TO_BORDER
#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
#endif
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_texBufferWidth, m_texBufferHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		// attach depth texture as FBO's depth buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TexId[0], 0);
		glDrawBuffers(0, GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE)
			LOGE("ERROR!! RenderTargetType_DEPTH is not complete!\n");

		break;
	case RenderTargetType_COLOR:
		LOGI("Create RenderTargetType_COLOR: %d, %d\n", texWidth, texHeight);
		glGenFramebuffers(1, &m_FBOId[0]);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId[0]);
		glGenTextures(1, &m_TexId[0]);
		glBindTexture(GL_TEXTURE_2D, m_TexId[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_texBufferWidth, m_texBufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexId[0], 0);

		// create depth buffer (renderbuffer)
		glGenRenderbuffers(1, &m_rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_texBufferWidth, m_texBufferHeight);
		// attach buffers

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE)
			LOGE("ERROR!! RenderTargetType_COLOR is not complete!\n");

		break;
	case RenderTargetType_COLOR_MULTISAMPLED:
#ifdef _WINDOWS
		LOGI("Create RenderTargetType_COLOR_MULTISAMPLED: %d, %d\n", texWidth, texHeight);
		// configure MSAA framebuffer
		glGenFramebuffers(1, &m_FBOId[0]);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId[0]);
		glGenTextures(1, &m_TexId[0]);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_TexId[0]);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, m_texBufferWidth, m_texBufferHeight, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_TexId[0], 0);

		glGenRenderbuffers(1, &m_rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, m_texBufferWidth, m_texBufferHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE)
			LOGE("ERROR!! RenderTargetType_COLOR_MULTISAMPLED is not complete!\n");
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
			LOGE("ERROR!! RenderTargetType_COLOR_MULTISAMPLED screenTexture is not complete!\n");
#endif
		break;
	case RenderTargetType_COLOR_BRIGHTNESS:
		LOGI("Create RenderTargetType_COLOR_BRIGHTNESS: %d, %d\n", texWidth, texHeight);
		// configure MSAA framebuffer
		glGenFramebuffers(1, &m_FBOId[0]);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId[0]);
		glGenTextures(2, m_TexId);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, m_TexId[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_texBufferWidth, m_texBufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_TexId[i], 0);
		}
		glGenRenderbuffers(1, &m_rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_texBufferWidth, m_texBufferHeight);
		// attach buffers
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);


		glDrawBuffers(2, attachments);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE)
			LOGE("ERROR!! RenderTargetType_COLOR_BRIGHTNESS is not complete!\n");
		break;
	case RenderTargetType_COLOR_BLURRING:
		LOGI("Create RenderTargetType_COLOR_BLURRING!\n");
		glGenFramebuffers(2, m_FBOId);
		glGenTextures(2, m_TexId);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId[i]);
			glBindTexture(GL_TEXTURE_2D, m_TexId[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_texBufferWidth, m_texBufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexId[i], 0);
			// also check if framebuffers are complete (no need for depth buffer)
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				LOGE("RenderTargetType_COLOR_BLURRING [%d] not complete!", i);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		break;
	}

	m_initialized = true;
	return true;
}

void RenderTarget::BeginRender(const char* shadername)
{
	if (!m_initialized)
	{
		LOGE("ERROR! RenderTarget not init!!!!!");
		return;
	}

	m_isEnable = true;

	//if (shadername != NULL)
	//	ShaderManager::getInstance()->setUseProgram(shadername);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId[0]);
	glViewport(0, 0, m_texBufferWidth, m_texBufferHeight);
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);
}

GLuint RenderTarget::EndRender()
{
	if (!m_initialized || !m_isEnable)
	{
		return 0;
	}

	m_isEnable = false;

	if (m_type == RenderTargetType_COLOR_MULTISAMPLED)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBOId[0]);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glBlitFramebuffer(0, 0, m_texBufferWidth, m_texBufferHeight, 0, 0, m_texBufferWidth, m_texBufferHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_appcontext->GetWindowWidth(), m_appcontext->GetWindowHeight());

	if (m_type == RenderTargetType_COLOR_MULTISAMPLED)
		return screenTexture;
	else
		return m_TexId[0];
}

GLuint RenderTarget::GetTextureId(int index)
{
	return m_TexId[index];
}

void RenderTarget::RenderDebug()
{
	if (m_type == RenderTargetType_COLOR_MULTISAMPLED)
		Debugging::getInstance()->DrawTex(screenTexture, "debugShader");
	else
		Debugging::getInstance()->DrawTex(m_TexId[0], "debugShader");
}

void RenderTarget::Render(bool useDefaultShader)
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

	if (m_type == RenderTargetType_COLOR_MULTISAMPLED)
		glBindTexture(GL_TEXTURE_2D, screenTexture);
	else
		glBindTexture(GL_TEXTURE_2D, m_TexId[0]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
GLuint RenderTarget::MakeBloom(GLuint BriTexture, unsigned int amount) // amount default = 4
{
	bool horizontal = true, first_iteration = true;
	glm::vec2 tex_offset = glm::vec2(1.0f / (float)m_texBufferWidth, 1.0f / (float)m_texBufferHeight);
	glViewport(0, 0, m_texBufferWidth, m_texBufferHeight);
	for (unsigned int i = 0; i < amount; i++)
	{
		if (horizontal)
			ShaderManager::getInstance()->setUseProgram("Blur_Horizontal");
		else
			ShaderManager::getInstance()->setUseProgram("Blur_Vertical");

		ShaderSet::setVec2("tex_offset", tex_offset);

		glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId[horizontal]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? BriTexture : m_TexId[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)

		if (quadVAO == 0) InitquadVAO();
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glViewport(0, 0, m_appcontext->GetWindowWidth(), m_appcontext->GetWindowHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return m_TexId[!horizontal];

	//ShaderManager::getInstance()->setUseProgram("bloom_Final");
	//glBindVertexArray(quadVAO);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, normalTexture);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, m_TexId[!horizontal]);
	//ShaderManager::getInstance()->setInt("bloom", 1);
	//ShaderManager::getInstance()->setInt("scene", 0);
	//ShaderManager::getInstance()->setInt("bloomBlur", 1);

	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//glBindVertexArray(0);
}

void RenderTarget::InitDefaultShader()
{
	const char * verShader = {
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec2 aTexCoords;\n"
		"out vec2 TexCoords;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	TexCoords = aTexCoords;\n"
		"	gl_Position =  vec4(aPos, 1.0);\n"
		"}\n"
	};
	const char *fragShader = {
		"precision highp float;\n"
		"\n"
		"in vec2 TexCoords;\n"
		"\n"
		"uniform sampler2D tex;\n"
		"\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = texture(tex, TexCoords);\n"
		"}\n"
	};

	m_default_shader.LoadShader(verShader, fragShader, true);
}

void RenderTarget::InitquadVAO()
{
	if (quadVAO == 0)
	{
		static float quadVertices[] = {
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
		glBindVertexArray(0);
	}
}

RenderTarget::RenderTarget()
	: m_initialized(false)
	, quadVAO(0)
	, m_isEnable(false)
{
}

RenderTarget::~RenderTarget()
{
	//glDeleteFramebuffers(1, m_FBOId);
	//if (m_type == FrameBufferType_DEPTH)
	//glDeleteTextures(1, m_TexId);
	//else
	//glDeleteTextures(2, m_TexId);
}