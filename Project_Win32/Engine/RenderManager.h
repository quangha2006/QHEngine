#pragma once
#include <iostream>
#include "Model.h"
#include "ModelManager.h"
#include "RenderTarget.h"
#include "AppContext.h"
#include "SkyBox.h"

class RenderManager
{
private:
	static RenderManager* instance;
	RenderTarget mShadowRT, mSenceRT, mBrightnessRT, mBluringRT;
	GLuint mDepthMapTexId, mSenceTexId, mBloom_bright;
	SkyBox *mSkybox;
	AppContext * mAppcontext;

	Shader m_default_shader;
	GLuint quadVAO, quadVBO;

	GLuint RenderDepthMap();
	GLuint RenderSence();
	GLuint PostProcessBloom(GLuint textsrc);
	void InitDefaultShader();
	void InitquadVAO();
	void RenderFinal();
public:
	static RenderManager* getInstance();
	void Init(AppContext * appcontext, Camera *camera);
	void Update();
	void Render();
	void SetSkyBox(SkyBox *skybox);
	
	GLuint GetDepthMapId();
	RenderManager();
	~RenderManager();
};

