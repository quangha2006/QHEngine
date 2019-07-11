#ifndef __RENDERMANAGER_H__
#define __RENDERMANAGER_H__

#include <iostream>
#include "Model.h"
#include "ModelManager.h"
#include "RenderTarget.h"
#include "AppContext.h"
#include "SkyBox.h"
#include "QHAxis.h"

class RenderManager
{
private:
	static RenderManager* instance;
	RenderTarget mShadowRT, mSenceRT, mBrightnessRT, mBluringRT;
	GLuint mDepthMapTexId, mSenceTexId, mBloomId;
	SkyBox *mSkybox;
	AppContext * mAppcontext;

	Shader m_default_shader;
	GLuint quadVAO, quadVBO;
	bool m_isEnableShadowMap, m_isEnableBloom;
	bool mGammaCorrection;
	GLuint RenderDepthMap();
	GLuint RenderSence();
	GLuint PostProcessBloom(GLuint textsrc);
	void InitDefaultShader();
	void InitquadVAO();
	void RenderFinal();

	QHAxis axis;
public:
	static RenderManager* getInstance();
	void Init(AppContext * appcontext, Camera *camera);
	void Update();
	void Render();
	void SetSkyBox(SkyBox *skybox);
	void SetEnableShadowMap(bool is_enable);
	void SetEnableBloom(bool is_enable);
	void SwitchBloomMode();
	void SwitchShadowMapMode();
	bool IsEnableShadow();
	bool IsEnableBloom();
	bool isEnablemGammaCorrection();
	GLuint GetDepthMapId();
	void GetGLViewport(int &width, int &height);
	RenderManager();
	~RenderManager();
};
#endif //!__RENDERMANAGER_H__