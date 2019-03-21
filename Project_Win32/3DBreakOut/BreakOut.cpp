//#include "stdafx.h"
#include "BreakOut.h"
#include "Utils.h"
#include "RenderManager.h"
#include "PhysicsSimulation.h"
#include "QHMath.h"

void BreakOut::Init()
{
	mCamera->Pos = glm::vec3(0.0f, 12.0f, 40.0f);
	mCamera->Target = glm::vec3(0.0f, 0.0f, 5.0f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	mCamera->lightPos = glm::vec3(15.0f, 10.5f, 15.0f);

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("3DBreakOutGame/floor/Tiles_Terracotta_002a_SD/untitled2.dae", false, false);
	m_Streetenvironment.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
	m_Streetenvironment.SetScale(glm::vec3(5.0, 1.0, 5.0));
	m_Streetenvironment.SetIsDrawDepthMap(false);
	m_Streetenvironment.CreateBoxShapePhysicsBody(0.0f, glm::vec3(30.0, .1, 30.0), glm::vec3(0., 0.001, 0.));
	m_Streetenvironment.GetRigidBody()->setFriction(0.0);
	m_Streetenvironment.GetRigidBody()->setRestitution(1.);

	cube.Init("3DBreakOutGame/UVCircle2.dae");
	cube.SetPos(glm::vec3(0.0f, 10.0f, 0.0f));
	cube.SetCustomColor(glm::vec3(0.0f, 0.0f, 1.0f));
	cube.SetRotate(45, glm::vec3(0.0, 1.0, 1.0));
	cube.CreateBoxShapePhysicsBody(1.0f, glm::vec3(0.52, 0.52, 0.52));

	top.Init("3DBreakOutGame/cube_05_radius.dae");
	top.SetPos(glm::vec3(0.0f, 0.8f, -30.0f));
	top.SetScale(glm::vec3(70.0, 4.0, 1.0));
	top.CreateBoxShapePhysicsBody(0.0f, glm::vec3(35., 20.52, 0.52));
	top.GetRigidBody()->setFriction(0.0);
	top.GetRigidBody()->setRestitution(1.);

	botton.Init("3DBreakOutGame/cube_05_radius.dae");
	botton.SetPos(glm::vec3(0.0f, 0.8f, 30.0f));
	botton.SetScale(glm::vec3(70.0, 4.0, 1.0));
	botton.CreateBoxShapePhysicsBody(0.0f, glm::vec3(35., 20.52, 0.52));
	botton.GetRigidBody()->setFriction(0.0);
	botton.GetRigidBody()->setRestitution(1.);

	left.Init("3DBreakOutGame/cube_05_radius.dae");
	left.SetPos(glm::vec3(-30.0f, 0.8f, 0.0f));
	left.SetScale(glm::vec3(1.0, 4.0, 70.0));
	left.CreateBoxShapePhysicsBody(0.0f, glm::vec3(.52, 20.52, 35.));
	left.GetRigidBody()->setFriction(0.0);
	left.GetRigidBody()->setRestitution(1.);

	right.Init("3DBreakOutGame/cube_05_radius.dae");
	right.SetPos(glm::vec3(30.0f, 0.8f, 0.0f));
	right.SetScale(glm::vec3(1.0, 4.0, 70.0));
	right.CreateBoxShapePhysicsBody(0.0f, glm::vec3(.52, 20.52, 35.));
	right.GetRigidBody()->setFriction(0.0);
	right.GetRigidBody()->setRestitution(1.);
	//int xxx = 0;
	//for (int k = 0; k < MaxBall; k++)
	//{
	//	for (int i = 0; i < MaxBall; i++)
	//	{
	//		for (int j = 0; j < MaxBall; j++)
	//		{
	//			float pos_x = 1.2 * i;
	//			float pos_y = 2 + 1.2 * k;
	//			float pos_z = 1.2 * j;

	//			listcube[xxx].Init("3DBreakOutGame/UVCircle2.dae");
	//			listcube[xxx].SetPos(glm::vec3(pos_x, pos_y, pos_z));
	//			listcube[xxx].SetCustomColor(Utils::RandomColor());
	//			//listcube[xxx].CreateBoxShapePhysicsBody(1.0f, glm::vec3(0.52, 0.52, 0.52), glm::vec3(0., 0., 0.));
	//			listcube[xxx].CreateSphereShapePhysiceBody(1., 1.);
	//			listcube[xxx].GetRigidBody()->setFriction(0.);
	//			listcube[xxx].GetRigidBody()->setRollingFriction(0.);
	//			listcube[xxx].GetRigidBody()->setSpinningFriction(0.);
	//			listcube[xxx].GetRigidBody()->setRestitution(1.);
	//			listcube[xxx].GetRigidBody()->applyForce(btVector3(pos_x * 10., pos_y  * -500., pos_z), btVector3(0.0, 0.0, 0.0));
	//			xxx++;
	//		}
	//	}
	//}

	uvcircle.Init("3DBreakOutGame/UVCircle2.dae");
	uvcircle.SetScale(glm::vec3(1.0));
	uvcircle.SetPos(glm::vec3(0.0f, 200.5f, 0.0f));
	uvcircle.CreateSphereShapePhysiceBody(1., 1.);
	
	uvcircle.GetRigidBody()->setFriction(0.);
	uvcircle.GetRigidBody()->setRollingFriction(0.);
	uvcircle.GetRigidBody()->setSpinningFriction(0.);
	uvcircle.GetRigidBody()->setRestitution(1.);
}

void BreakOut::Update()
{
	static bool firstUpdate = true;
	if (firstUpdate)
	{
		int xxx = 0;
		for (int k = 0; k < MaxBall; k++)
		{
			for (int i = 0; i < MaxBall; i++)
			{
				for (int j = 0; j < MaxBall; j++)
				{
					float pos_x = 1.2 * i;
					float pos_y = 2 + 1.2 * k;
					float pos_z = 1.2 * j;

					listcube[xxx] = cube;
					listcube[xxx].SetPos(glm::vec3(pos_x, pos_y, pos_z));
					listcube[xxx].SetCustomColor(Utils::RandomColor());
					listcube[xxx].CreateSphereShapePhysiceBody(1., 1.);
					listcube[xxx].GetRigidBody()->setFriction(0.);
					listcube[xxx].GetRigidBody()->setRollingFriction(0.);
					listcube[xxx].GetRigidBody()->setSpinningFriction(0.);
					listcube[xxx].GetRigidBody()->setRestitution(1.);
					listcube[xxx].GetRigidBody()->applyForce(btVector3(pos_x * 100., -pos_y  * 300., pos_z), btVector3(0.0, 0.0, 0.0));
					xxx++;
				}
			}
		}
	}
	firstUpdate = false;
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
	btRigidBody* uvcircle_RigidBody;
	glm::mat4 lightmatrix;
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
	case 'K':
		//lightmatrix = glm::rotate(mCamera->lightSpaceMatrix, 1.0f, glm::vec3(0., 1., 0.));
		//mCamera->lightPos = mCamera->ExtractCameraPos(lightmatrix);
		PhysicsSimulation::getInstance()->SetGravity(btVector3(0., -20., 0.));
		break;
	case 'L':
		//lightmatrix = glm::rotate(mCamera->lightSpaceMatrix, -1.0f, glm::vec3(0., 1., 0.));
		//mCamera->lightPos = mCamera->ExtractCameraPos(lightmatrix);
		PhysicsSimulation::getInstance()->SetGravity(btVector3(0., 20., 0.));
		mCamera->light_near--;
		break;
	case 'D':
		uvcircle_RigidBody = uvcircle.GetRigidBody();
		uvcircle_RigidBody->applyForce(btVector3(1000.0, 0., 500.), btVector3(0.0, 0.0, 0.0));
		break;
	case 'A':
		uvcircle_RigidBody = uvcircle.GetRigidBody();
		uvcircle_RigidBody->applyForce(btVector3(-1000., 0., -500.), btVector3(0.0, 0.0, 0.0));
		break;
	case 'S':
		uvcircle_RigidBody = uvcircle.GetRigidBody();
		uvcircle_RigidBody->applyForce(btVector3(500., 0., 1000.), btVector3(0.0, 0.0, 0.0));
		break;
	case 'W':
		uvcircle_RigidBody = uvcircle.GetRigidBody();
		uvcircle_RigidBody->applyForce(btVector3(-500., 0., -1000.0), btVector3(0.0, 0.0, 0.0));
		
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