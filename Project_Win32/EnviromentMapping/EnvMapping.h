#pragma once
#include <AppBase.h>
#include <Model.h>
#include <SkyBox.h>
#include <Logs.h>
#include <Camera.h>
#include <FrameBuffer.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class EnvMapping : public AppBase
{
private:
	Model mNanosuit;
	SkyBox mSkyBox;
	FrameBuffer mframebuffer;
public:
	void Draw();
	void Init();
	void GetRequireScreenSize(int32_t &width, int32_t &height);
	void OnGameKeyPressed(int key, int scancode, int action, int mods);
	EnvMapping();
	~EnvMapping();
};

