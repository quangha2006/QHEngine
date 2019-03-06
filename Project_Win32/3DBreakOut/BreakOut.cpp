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
	m_Streetenvironment.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
	m_Streetenvironment.SetIsDrawDepthMap(false);

	cube.Init("3DBreakOutGame/cube2.dae");
	cube.SetPos(glm::vec3(2.0f, 10.0f, 0.0f));
	cube.SetCustomColor(glm::vec3(0.0f, 0.0f, 1.0f));

	//for (int i = 0; i < 20; i++)
	//{
	//	listcube[i] = cube;
	//	listcube[i].SetTranslate(glm::vec3(0.0f, (float)i, 0.0f));
	//	listcube[i].SetCustomColor(glm::vec3((float)i/10, 0.0f, 1.0f));
	//}

	uvcircle.Init("3DBreakOutGame/UVCircle.dae");
	uvcircle.SetPos(glm::vec3(0.0f, 0.0f, 5.0f));
}

void BreakOut::Update()
{

	//cube.SetPos(glm::vec3(x1, y1, z1));
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