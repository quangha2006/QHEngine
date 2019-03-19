#include "stdafx.h"
#include "BreakOut.h"
#include "Utils.h"
#include "RenderManager.h"

void BreakOut::Init()
{
	mCamera->Pos = glm::vec3(0.0f, 12.0f, 20.0f);
	mCamera->Target = glm::vec3(0.0f, 0.0f, 5.0f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	mCamera->lightPos = glm::vec3(15.0f, 10.5f, 15.0f);

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("3DBreakOutGame/floor/Tiles_Terracotta_002a_SD/untitled2.dae", false, false);
	m_Streetenvironment.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
	m_Streetenvironment.SetScale(glm::vec3(5.0, 1.0, 5.0));
	m_Streetenvironment.SetIsDrawDepthMap(false);
	m_Streetenvironment.CreateBoxShapePhysicsBody(0.0f, glm::vec3(30.0, .1, 30.0), glm::vec3(0., 0.001, 0.));

	cube.Init("3DBreakOutGame/cube_05_radius.dae");
	cube.SetPos(glm::vec3(0.0f, 10.0f, 0.0f));
	cube.SetCustomColor(glm::vec3(0.0f, 0.0f, 1.0f));
	cube.SetRotate(45, glm::vec3(0.0, 1.0, 1.0));
	cube.CreateBoxShapePhysicsBody(1.0f, glm::vec3(0.52, 0.52, 0.52));

	top.Init("3DBreakOutGame/cube_05_radius.dae");
	top.SetPos(glm::vec3(0.0f, 0.8f, -30.0f));
	top.SetScale(glm::vec3(70.0, 1.0, 1.0));
	top.CreateBoxShapePhysicsBody(0.0f, glm::vec3(35., 0.52, 0.52));

	botton.Init("3DBreakOutGame/cube_05_radius.dae");
	botton.SetPos(glm::vec3(0.0f, 0.8f, 30.0f));
	botton.SetScale(glm::vec3(70.0, 1.0, 1.0));
	botton.CreateBoxShapePhysicsBody(0.0f, glm::vec3(35., 0.52, 0.52));

	left.Init("3DBreakOutGame/cube_05_radius.dae");
	left.SetPos(glm::vec3(-30.0f, 0.8f, 0.0f));
	left.SetScale(glm::vec3(1.0, 1.0, 70.0));
	left.CreateBoxShapePhysicsBody(0.0f, glm::vec3(.52, 0.52, 35.));

	right.Init("3DBreakOutGame/cube_05_radius.dae");
	right.SetPos(glm::vec3(30.0f, 0.8f, 0.0f));
	right.SetScale(glm::vec3(1.0, 1.0, 70.0));
	right.CreateBoxShapePhysicsBody(0.0f, glm::vec3(.52, 0.52, 35.));

	int xxx = 0;
	for (int k = 0; k < 5; k++)
	{
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				listcube[xxx].Init("3DBreakOutGame/cube_05_radius.dae");
				listcube[xxx].SetPos(glm::vec3(1.01 * i, 10 + 1.01 * k, 1.01 * j));
				listcube[xxx].SetCustomColor(Utils::RandomColor());
				listcube[xxx].CreateBoxShapePhysicsBody(1.0f, glm::vec3(0.52, 0.52, 0.52), glm::vec3(0., 0., 0.));
				xxx++;
			}
		}
	}

	uvcircle.Init("3DBreakOutGame/UVCircle2.dae");
	uvcircle.SetScale(glm::vec3(2.0));
	uvcircle.SetPos(glm::vec3(0.0f, 4.0f, 0.0f));
	uvcircle.CreateSphereShapePhysiceBody(1.0, 2.);
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
		break;
	case 'M':
		RenderManager::getInstance()->SwitchShadowMapMode();
		break;
	case 'U':
		m_Streetenvironment.SetRotate(2, glm::vec3(0., 0., 1.));
		break;
	case 'H':
		m_Streetenvironment.SetRotate(-2, glm::vec3(0., 0., 1.));
		break;
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