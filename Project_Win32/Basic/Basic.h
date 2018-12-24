#pragma once
#include <AppBase.h>
#include <AppContext.h>
#include <Shader.h>
#include <Model.h>
#include <Camera.h>
#include <ShaderManager.h>
#include <FrameBuffer.h>
#include "SkyBox.h"
#include "Sound.h"
#include "QHAxis.h"

#include <thread>

class Basic : public AppBase
{
private:
	Model mNanosuit, m_Streetenvironment, mMerce, mSpider, saberclass, mGallacticCruiser, mMonster_1;
	FrameBuffer mframebuffer, HDRBuffer;
	SkyBox mSkyBox;
	float timestamp_for_lamp;
	Sound soundIntro;
	QHAxis axis;

	bool m_initialized;;
	
public:
	void Draw();
	void Init();
	void LoadingThread(ShareContext shared_context);
	void GetRequireScreenSize(int32_t &width, int32_t &height);
	void OnGameKeyPressed(int key, int scancode, int action, int mods);
	Basic();
	~Basic();
};