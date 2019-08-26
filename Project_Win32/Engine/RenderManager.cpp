#include "RenderManager.h"
#include "Debugging.h"
#include "UserInterface.h"

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

	ShaderManager::getInstance()->LoadFromFile("model_skinning", "Shaders/model_loading.vs", "Shaders/model_loading.fs", "#define SKINNED");
	ShaderManager::getInstance()->LoadFromFile("model", "Shaders/model_loading.vs", "Shaders/model_loading.fs");
	ShaderManager::getInstance()->LoadFromFile("debugShader", "Shaders/framebuffers_debug.vs", "Shaders/framebuffers_debug.fs"); // For debug
	ShaderManager::getInstance()->LoadFromFile("depthShader_skinning", "Shaders/DepthShader.vs", "Shaders/DepthShader.fs", "#define SKINNED");
	ShaderManager::getInstance()->LoadFromFile("depthShader", "Shaders/DepthShader.vs", "Shaders/DepthShader.fs");
	ShaderManager::getInstance()->LoadFromFile("basic", "Shaders/BasicVS.vs", "Shaders/BasicFS.fs");
	ShaderManager::getInstance()->LoadFromFile("Brightness", "Shaders/BasicVS.vs", "Shaders/brightness.fs", "#define DOWNFILTER");
	ShaderManager::getInstance()->LoadFromFile("Blur_Horizontal", "Shaders/BasicVS.vs", "Shaders/blur.fs", "#define HORIZONTAL");
	ShaderManager::getInstance()->LoadFromFile("Blur_Vertical", "Shaders/BasicVS.vs", "Shaders/blur.fs", "#define VERTICAL");
	ShaderManager::getInstance()->LoadFromFile("bloom_Final", "Shaders/BasicVS.vs", "Shaders/bloom_final.fs");
	ShaderManager::getInstance()->LoadFromFile("debugPhysics", "Shaders/DebugPhysics.vs", "Shaders/DebugPhysics.fs");

	mShadowRT.Init(mAppcontext, RenderTargetType_DEPTH, 2048, 2048);

	int width = mAppcontext->GetWindowWidth();
	int height = mAppcontext->GetWindowHeight();
#ifdef WIN32
	mSenceRT.Init(mAppcontext, RenderTargetType_COLOR_MULTISAMPLED, width, height);
#else // Android
	mSenceRT.Init(mAppcontext, RenderTargetType_COLOR, width, height);
#endif
	mBrightnessRT.Init(mAppcontext, RenderTargetType_COLOR, width/2, height/2);

	mBluringRT.Init(mAppcontext, RenderTargetType_COLOR_BLURRING, width/2, height/2);

	InitquadVAO();
	InitDefaultShader();
	//glEnable(GL_CULL_FACE);
	//axis.Init(camera);
}

void RenderManager::Update()
{

}

void RenderManager::Render()
{
	mDepthMapTexId = RenderDepthMap();

	mSenceTexId = RenderSence();

	mBloomId =  PostProcessBloom(mSenceTexId);

	RenderFinal();

	//debug
	//Debugging::getInstance()->DrawTex(mDepthMapTexId, "debugShader");
	//Debugging::getInstance()->DrawTex(mBloomId, "debugShader");
	//Debugging::getInstance()->DrawTex(mSenceTexId, "debugShader");
	//Debugging::getInstance()->DrawTex(mBrightnessRT.GetTextureId(0), "debugShader");

	UserInterface::getInstance()->Render();
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

void RenderManager::SetAmountBloom(int amount)
{
	if (amount > 0)
		mAmountBlurBloom = amount;
}

void RenderManager::SwitchBloomMode()
{
	m_isEnableBloom = !m_isEnableBloom;
}

void RenderManager::SwitchShadowMapMode()
{
	m_isEnableShadowMap = !m_isEnableShadowMap;
}

bool RenderManager::IsEnableShadow()
{
	return m_isEnableShadowMap;
}

bool RenderManager::IsEnableBloom()
{
	return m_isEnableBloom;
}

bool RenderManager::isEnablemGammaCorrection()
{
	return mGammaCorrection;
}

GLuint RenderManager::RenderDepthMap()
{
	if (!m_isEnableShadowMap)
		return 0;
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glFrontFace(GL_CW);
	mShadowRT.BeginRender();

	ModelManager::getInstance()->Render(RenderTargetType_DEPTH);
	//glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	return mShadowRT.EndRender();
}

GLuint RenderManager::RenderSence()
{
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);

	Camera *mCamera = Camera::getInstance();

	mSenceRT.BeginRender();

	mSkybox->Draw(mCamera);

	ModelManager::getInstance()->Render(RenderTargetType_COLOR);
	axis.Draw();
	PhysicsSimulation::getInstance()->RenderPhysicsDebug();
	return mSenceRT.EndRender();
}

GLuint RenderManager::PostProcessBloom(GLuint textsrc)
{
	if (!m_isEnableBloom)
		return 0;

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	mBrightnessRT.BeginRender();
	ShaderManager::getInstance()->setUseProgram("Brightness");
	glm::vec2 tex_offset = glm::vec2(1.0f/ (float)mSenceRT.GetWidth(), 1.0f / (float)mSenceRT.GetHeight());
	ShaderSet::setVec2("u_TexelOffsets", tex_offset);
	mSenceRT.Render();
	GLuint brightnessRT = mBrightnessRT.EndRender();

	GLuint blurbloomRT = mBluringRT.MakeBloom(brightnessRT, mAmountBlurBloom);

	return blurbloomRT;
}

void RenderManager::RenderFinal()
{

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	ShaderManager::getInstance()->setUseProgram("bloom_Final");
	//m_default_shader.use();

	glBindVertexArray(quadVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mSenceTexId);

	if (m_isEnableBloom)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mBloomId);
	}

	ShaderSet::setInt("txScene", 0);
	ShaderSet::setInt("txBloomBlur", 1);
	ShaderSet::setInt("isEnablebloom", m_isEnableBloom);
	ShaderSet::setFloat("exposure", 1.0f);
	ShaderSet::setInt("GammaCorrection", mGammaCorrection);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	CheckGLError("RenderFinal");
}

GLuint RenderManager::GetDepthMapId()
{
	return mDepthMapTexId;
}

void RenderManager::GetGLViewport(int & width, int & height)
{
	width = mAppcontext->GetWindowWidth();
	height = mAppcontext->GetWindowHeight();
}

int RenderManager::GetAmountBloom()
{
	return mAmountBlurBloom;
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
	: mDepthMapTexId(0)
	, quadVAO(0)
	, m_isEnableShadowMap(true)
	, m_isEnableBloom(true)
	, mGammaCorrection(true)
	, mAmountBlurBloom(2)
{
}


RenderManager::~RenderManager()
{
}
