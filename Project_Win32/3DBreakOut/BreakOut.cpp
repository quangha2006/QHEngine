#include "stdafx.h"
#include "BreakOut.h"
#include "Utils.h"
#include "RenderManager.h"

void BreakOut::Init()
{
	mCamera->Pos = glm::vec3(0.0f, 12.0f, 20.0f);
	mCamera->Target = glm::vec3(0.0f, 0.0f, 5.0f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	mCamera->lightPos = glm::vec3(13.0f, 15.0f, 14.0f);

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("Streetenvironment/Street environment_V01.obj", true, false);
	m_Streetenvironment.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
	m_Streetenvironment.SetIsDrawDepthMap(false);
	m_Streetenvironment.CreatePhysicsBody(0.0f, glm::vec3(30.0, 0.1, 30.0));

	cube.Init("3DBreakOutGame/cube2.dae");
	cube.SetPos(glm::vec3(0.0f, 10.0f, 0.0f));
	cube.SetCustomColor(glm::vec3(0.0f, 0.0f, 1.0f));
	cube.SetRotate(30, glm::vec3(1.0, 0.0, 0.0));
	cube.CreatePhysicsBody(1.0f, glm::vec3(2.15, 0.22, 0.44), glm::vec3(0., .44, 0.));

	for (int i = 0; i < 50; i++)
	{
		listcube[i].Init("3DBreakOutGame/cube2.dae"); 
		listcube[i].SetPos(glm::vec3(2.0f + i*0.1, (float)i, 0.0f + 0*0.2));
		listcube[i].SetCustomColor(Utils::RandomColor());
		listcube[i].CreatePhysicsBody(1.0f, glm::vec3(2.15, 0.22, 0.44), glm::vec3(0., .44, 0.));
	}

	uvcircle.Init("3DBreakOutGame/UVCircle.dae");
	uvcircle.SetPos(glm::vec3(0.0f, 0.0f, 5.0f));
}

void BreakOut::Update()
{

}

void BreakOut::GetRequireScreenSize(int32_t & width, int32_t & height)
{
	width = 960;
	height = 540;
}

void BreakOut::OnGameKeyPressed(int key, int scancode, int action, int mods)
{
	char c = (char)key;
	if (action == 0) return;
	switch (c)
	{
	case 'B':
		RenderManager::getInstance()->SwitchBloomMode();
		return;
	}
}

BreakOut::BreakOut()
{
}


BreakOut::~BreakOut()
{

}
AppBase* AppFactory()
{
	return new BreakOut();
}