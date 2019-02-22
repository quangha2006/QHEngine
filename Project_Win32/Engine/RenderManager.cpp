#include "RenderManager.h"
#include "Debugging.h"
RenderManager * RenderManager::instance = NULL;


RenderManager * RenderManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new RenderManager();
	}
	return instance;
}

void RenderManager::Init(AppContext * appcontext, Camera *camera)
{
	mAppcontext = appcontext;

	ShaderManager::getInstance()->Init("model", "Shaders/model_loading.vs", "Shaders/model_loading.fs");
	ShaderManager::getInstance()->Init("debugShader", "Shaders/framebuffers_debug.vs", "Shaders/framebuffers_debug.fs"); // For debug
	ShaderManager::getInstance()->Init("depthShader", "Shaders/DepthShader.vs", "Shaders/DepthShader.fs");
	ShaderManager::getInstance()->Init("basic", "Shaders/BasicVS.vs", "Shaders/BasicFS.fs");
	ShaderManager::getInstance()->Init("Brightness", "Shaders/BasicVS.vs", "Shaders/brightness.fs");
	ShaderManager::getInstance()->Init("Blur_Horizontal", "Shaders/BasicVS.vs", "Shaders/Blur_Horizontal.fs");
	ShaderManager::getInstance()->Init("Blur_Vertical", "Shaders/BasicVS.vs", "Shaders/Blur_Vertical.fs");
	ShaderManager::getInstance()->Init("bloom_Final", "Shaders/BasicVS.vs", "Shaders/bloom_final.fs");

	mShadowRT.Init(mAppcontext, RenderTargetType_DEPTH, 2048, 2048);

	//mSenceRT.Init(mAppcontext, RenderTargetType_COLOR_MULTISAMPLED, mAppcontext->GetWindowWidth(), mAppcontext->GetWindowHeight());

	mSenceRT.Init(mAppcontext, RenderTargetType_COLOR, mAppcontext->GetWindowWidth(), mAppcontext->GetWindowHeight());

	mBrightnessRT.Init(mAppcontext, RenderTargetType_COLOR, mAppcontext->GetWindowWidth(), mAppcontext->GetWindowHeight());

	mBluringRT.Init(mAppcontext, RenderTargetType_COLOR_BLURRING, mAppcontext->GetWindowWidth(), mAppcontext->GetWindowHeight());

	InitquadVAO();
	InitDefaultShader();
}

void RenderManager::Update()
{
	ModelManager::getInstance()->Update();
}

void RenderManager::Render()
{
	mDepthMapTexId = RenderDepthMap();

	mSenceTexId = RenderSence();

	mBloom_bright =  PostProcessBloom(mSenceTexId);

	RenderFinal();
}

void RenderManager::SetSkyBox(SkyBox * skybox)
{
	mSkybox = skybox;
}

void RenderManager::SetEnableShadowMap(bool is_enable)
{
	m_isEnableShadowMap = is_enable;
}

void RenderManager::SetEnableBloom(bool is_enable)
{
	m_isEnableBloom = is_enable;
}

void RenderManager::SwitchBloomMode()
{
	m_isEnableBloom = !m_isEnableBloom;
}

void RenderManager::SwitchShadowMapMode()
{
	m_isEnableShadowMap = !m_isEnableShadowMap;
}

GLuint RenderManager::RenderDepthMap()
{
	if (!m_isEnableShadowMap)
		return -1;

	Camera *mCamera = Camera::getInstance();
	mShadowRT.Enable("depthShader");
	ShaderManager::getInstance()->setMat4("lightSpaceMatrix", mCamera->lightSpaceMatrix);
	ShaderManager::getInstance()->setFloat("near_plane", mCamera->light_near);
	ShaderManager::getInstance()->setFloat("far_plane", mCamera->light_far);

	ModelManager::getInstance()->Render(RenderMode_Depth);
	return mShadowRT.Disable();
}

GLuint RenderManager::RenderSence()
{
	Camera *mCamera = Camera::getInstance();

	ShaderManager::getInstance()->setUseProgram("model");

	if (m_isEnableBloom)
		mSenceRT.Enable();

	mSkybox->Draw(mCamera);

	ModelManager::getInstance()->Render(RenderMode_Sence);

	return mSenceRT.Disable();
}

GLuint RenderManager::PostProcessBloom(GLuint textsrc)
{
	if (!m_isEnableBloom)
		return -1;

	mBrightnessRT.Enable();
	ShaderManager::getInstance()->setUseProgram("Brightness");
	mSenceRT.Render();
	GLuint brightnessRT = mBrightnessRT.Disable();

	return mBluringRT.MakeBloom(brightnessRT);
}

void RenderManager::RenderFinal()
{
	if (!m_isEnableBloom)
		return;

	ShaderManager::getInstance()->setUseProgram("bloom_Final");
	//m_default_shader.use();

	glBindVertexArray(quadVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mSenceTexId);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mBloom_bright);

	ShaderManager::getInstance()->setInt("bloom", 1);
	ShaderManager::getInstance()->setInt("scene", 0);
	ShaderManager::getInstance()->setInt("bloomBlur", 1);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	CheckGLError("RenderFinal");
}

GLuint RenderManager::GetDepthMapId()
{
	return mDepthMapTexId;
}

void RenderManager::InitDefaultShader()
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
		"uniform sampler2D scene;\n"
		"\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = texture(scene, TexCoords);\n"
		"}\n"
	};

	m_default_shader.LoadShader(verShader, fragShader, true);
}

void RenderManager::InitquadVAO()
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
	glBindVertexArray(0);
}


RenderManager::RenderManager()
{
	mDepthMapTexId = -1;
	quadVAO = -1;
	m_isEnableShadowMap = true;
	m_isEnableBloom = true;
}


RenderManager::~RenderManager()
{
}
