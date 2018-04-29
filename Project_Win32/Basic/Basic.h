#pragma once
#include <AppBase.h>
#include <Shader.h>
#include <Model.h>
#include <Camera.h>
#include <ShaderManager.h>
#include <FrameBuffer.h>
#include "SkyBox.h"

class Basic : public AppBase
{
private:
	Model mNanosuit, m_Streetenvironment, mMerce, mSun, mSpider, saberclass, mGallacticCruiser, mMonster_1;
	glm::mat4 Pos_model_Original;
	FrameBuffer mframebuffer;
	SkyBox mSkyBox;
public:
	void Draw();
	void Init();
	void GetRequireScreenSize(int32_t &width, int32_t &height);
	Basic();
	~Basic();
};