#include "stdafx.h"
#include "BreakOut.h"
#include "Utils.h"
void BreakOut::Init()
{
	mCamera->Pos = glm::vec3(0.0f, 12.0f, 20.0f);
	mCamera->Target = glm::vec3(0.0f, 0.0f, 5.0f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	mCamera->lightPos = glm::vec3(8.2f, 10.0f, 9.0f);

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("Streetenvironment/Street environment_V01.obj", true, false);
	m_Streetenvironment.SetPos(glm::vec3(0.0f, -1.0f, 0.0f));
	m_Streetenvironment.SetIsDrawDepthMap(false);
	m_Streetenvironment.CreatePhysicsBody(0.0f, glm::vec3(50.0, 1.0, 50.0));

	cube.Init("3DBreakOutGame/cube2.dae");
	cube.SetPos(glm::vec3(2.0f, 10.0f, 0.0f));
	cube.SetRotate(90, glm::vec3(0., 0., 1.));
	cube.SetCustomColor(glm::vec3(0.0f, 0.0f, 1.0f));

	cube.CreatePhysicsBody(1.0f, glm::vec3(2.25, 0.2, 0.5));

	for (int i = 0; i < 50; i++)
	{
		listcube[i].Init("3DBreakOutGame/cube2.dae"); 
		listcube[i].SetPos(glm::vec3(2.0f + i*0.1, (float)i, 0.0f + i*0.2));
		glm::vec3 color = Utils::RandomColor();
		listcube[i].SetCustomColor(color);

		glm::mat4 cubestartTransform = glm::translate(glm::mat4(), glm::vec3(2.0f, (float)i, 0.0f));
		listcube[i].CreatePhysicsBody(1.0f, glm::vec3(2.25, 0.2, 0.5));
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