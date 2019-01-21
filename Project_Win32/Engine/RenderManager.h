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
	GLuint mDepthMapRT;
	SkyBox *mSkybox;
	AppContext * mAppcontext;
public:
	static RenderManager* getInstance();
	void Init(AppContext * appcontext, Camera *camera);
	void Update();
	void Render();
	void SetSkyBox(SkyBox *skybox);
	GLuint RenderDepthMap();
	GLuint GetDepthMapId();
	RenderManager();
	~RenderManager();
};

