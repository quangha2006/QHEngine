#pragma once
#include <iostream>
#include "Model.h"
#include "ModelManager.h"
#include "RenderTarget.h"
#include "AppContext.h"

class RenderManager
{
private:
	static RenderManager* instance;
	RenderTarget mShadowRT, mSenceRT, mBrightnessRT, mBluringRT;

	AppContext * mAppcontext;
	Camera *mCamera;
public:
	static RenderManager* getInstance();
	void Init(AppContext * appcontext, Camera *camera);
	void Update();
	void Render();
	RenderManager();
	~RenderManager();
};

