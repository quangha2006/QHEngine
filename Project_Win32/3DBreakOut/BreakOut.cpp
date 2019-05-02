//#include "stdafx.h"
#include "BreakOut.h"
#include "Utils.h"
#include "RenderManager.h"
#include "PhysicsSimulation.h"
#include "QHMath.h"

float moving = 0.;

void BreakOut::Init()
{
	mCamera->Pos = glm::vec3(0.0f, 65.0f, 50.0f);
	mCamera->Target = glm::vec3(0.0f, 0.0f, 0.1f);
	mCamera->view = glm::lookAt(mCamera->Pos, mCamera->Target, mCamera->up);
	mCamera->lightPos = glm::vec3(15.0f, 10.5f, 15.0f);

	mSkyBox.Init("SkyBox");

	m_Streetenvironment.Init("3DBreakOutGame/floor/Tiles_Terracotta_002a_SD/untitled2.dae", false, false);
	m_Streetenvironment.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
	m_Streetenvironment.SetScale(glm::vec3(5.0, 1.0, 5.0));
	m_Streetenvironment.SetIsDrawDepthMap(false);
	m_Streetenvironment.CreateBoxShapePhysicsBody(0.0f, glm::vec3(40.0, .1, 40.0), glm::vec3(0., 0.001, 0.));
	m_Streetenvironment.GetRigidBody()->setFriction(0.0);
	m_Streetenvironment.GetRigidBody()->setRestitution(1.);

	cube.Init("3DBreakOutGame/cube_05_radius.dae");
	cube.SetPos(glm::vec3(0., 2., 30.5));
	cube.SetCustomColor(glm::vec3(1., 0., 0.));
	cube.SetScale(glm::vec3(11., 2., 2.));
	cube.CreateBoxShapePhysicsBody(0., glm::vec3(5.52, 2., 1.));
	cube.GetRigidBody()->setFriction(0.);
	cube.GetRigidBody()->setRollingFriction(0.);
	cube.GetRigidBody()->setSpinningFriction(0.);
	cube.GetRigidBody()->setRestitution(1.);

	top.Init("3DBreakOutGame/cube_05_radius.dae");
	top.SetPos(glm::vec3(0.0f, 0.8f, -35.0f));
	top.SetScale(glm::vec3(70.0, 4.0, 1.0));
	top.CreateBoxShapePhysicsBody(0.0f, glm::vec3(35., 20.52, 5.52),glm::vec3(0.0, 0.0, -2.5));
	top.GetRigidBody()->setFriction(0.0);
	top.GetRigidBody()->setRestitution(1.);

	botton.Init("3DBreakOutGame/cube_05_radius.dae");
	botton.SetPos(glm::vec3(0.0f, 0.8f, 35.0f));
	botton.SetScale(glm::vec3(70.0, 4.0, 1.0));
	botton.CreateBoxShapePhysicsBody(0.0f, glm::vec3(35., 20.52, 5.52), glm::vec3(0.0, 0.0, 2.5));
	botton.GetRigidBody()->setFriction(0.0);
	botton.GetRigidBody()->setRestitution(1.);

	left.Init("3DBreakOutGame/cube_05_radius.dae");
	left.SetPos(glm::vec3(-35.0f, 0.8f, 0.0f));
	left.SetScale(glm::vec3(1.0, 4.0, 70.0));
	left.CreateBoxShapePhysicsBody(0.0f, glm::vec3(5.52, 20.52, 35.), glm::vec3(-2.5, 0.0, 0.0));
	left.GetRigidBody()->setFriction(0.0);
	left.GetRigidBody()->setRestitution(1.);

	right.Init("3DBreakOutGame/cube_05_radius.dae");
	right.SetPos(glm::vec3(35.0f, 0.8f, 0.0f));
	right.SetScale(glm::vec3(1.0, 4.0, 70.0));
	right.CreateBoxShapePhysicsBody(0.0f, glm::vec3(5.52, 20.52, 35.), glm::vec3(2.5, 0.0, 0.0));
	right.GetRigidBody()->setFriction(0.0);
	right.GetRigidBody()->setRestitution(1.);

	int xxx = 0;
	for (int x = 0; x < 4; x++)
	{
		for (int z = 0; z < 5; z++)
		{
			float pos_x = 14. * x;
			float pos_z = 4. * z;

			listcube[xxx].Init("3DBreakOutGame/cube_05_radius.dae");
			listcube[xxx].SetPos(glm::vec3(pos_x - 20, 2., pos_z - 25));
			listcube[xxx].SetCustomColor(Utils::RandomColor());
			listcube[xxx].SetScale(glm::vec3(11., 2., 2.));
			listcube[xxx].CreateBoxShapePhysicsBody(0., glm::vec3(5.52, 2., 1.));
			listcube[xxx].GetRigidBody()->setFriction(0.);
			listcube[xxx].GetRigidBody()->setRollingFriction(0.);
			listcube[xxx].GetRigidBody()->setSpinningFriction(0.);
			listcube[xxx].GetRigidBody()->setRestitution(1.);
			xxx++;
		}
	}

	uvcircle.Init("3DBreakOutGame/UVCircle2.dae");
	uvcircle.SetScale(glm::vec3(1.0));
	uvcircle.SetPos(glm::vec3(0.0f, 1.f, 10.0f));
	uvcircle.CreateSphereShapePhysicsBody(1., 1.);
	
	//uvcircle.GetRigidBody()->setFriction(0.);
	uvcircle.GetRigidBody()->setRollingFriction(0.);
	uvcircle.GetRigidBody()->setSpinningFriction(0.);
	uvcircle.GetRigidBody()->setRestitution(1.);
	uvcircle.GetRigidBody()->applyCentralForce(btVector3(1000., 0., -5000.));

	mSoundCollistion.Init("Sound/Tink.wav");

	RenderManager::getInstance()->SetEnableBloom(false);
}

void BreakOut::Update(int delta)
{
	btCollisionObject* colObjA = (btCollisionObject*)cube.GetRigidBody();
	btCollisionObject* colObjB;

	// check collistion with botton bar
	{
		MyContactResultCallback result;
		colObjB = (btCollisionObject*)left.GetRigidBody();
		PhysicsSimulation::getInstance()->PhysicsStepCollision(colObjA, colObjB, result);
		if (result.m_connected && moving == -1.)
		{
			moving = 0.0f;
		}
	}

	// check collistion with botton bar
	{
		MyContactResultCallback result;
		colObjB = (btCollisionObject*)right.GetRigidBody();
		PhysicsSimulation::getInstance()->PhysicsStepCollision(colObjA, colObjB, result);
		if (result.m_connected && moving == 1.)
		{
			moving = 0.0f;
		}
	}

	cube.Translate(glm::vec3(moving, 0., 0.));

	for (int i = 0; i < 20; i++)
	{
		if (!listcube[i].GetIsVisible())
		{
			listcube[i].GetRigidBody()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}
	}

	colObjA = (btCollisionObject*)uvcircle.GetRigidBody();

	for (int i = 0; i < 20; i++)
	{
		if (!listcube[i].GetIsVisible()) continue;

		MyContactResultCallback result;
		colObjB = (btCollisionObject*)listcube[i].GetRigidBody();
		PhysicsSimulation::getInstance()->PhysicsStepCollision(colObjA, colObjB, result);
		if (result.m_connected)
		{
			listcube[i].SetCustomColor(Utils::RandomColor());
			mSoundCollistion.Play();
			listcube[i].SetVisible(false);
		}
	}

	// check collistion with botton bar
	{
		MyContactResultCallback result;
		colObjB = (btCollisionObject*)botton.GetRigidBody();
		PhysicsSimulation::getInstance()->PhysicsStepCollision(colObjA, colObjB, result);
		if (result.m_connected)
		{
			uvcircle.GetRigidBody()->forceActivationState(DISABLE_SIMULATION);
		}
	}
	// cheat
	glm::vec3 ballPos = uvcircle.GetPos();
	glm::vec3 cubePos = cube.GetPos();

	cubePos.x = ballPos.x;

	cube.SetPos(cubePos);

}

void BreakOut::GetRequireScreenSize(int32_t & width, int32_t & height)
{
	width = 960;
	height = 540;
}

bool BreakOut::OnGameKeyPressed(int key, int scancode, int action, int mods)
{
	char c = (char)key;
	//LOGI("%d\n", action);
	switch (key)
	{
	case 263:
		if (action == 1 || action == 2) moving = -1.0;
		if (action == 0) moving = 0.0;

		break;
	case 262:
		if (action == 1 || action == 2) moving = 1.0;
		if (action == 0) moving = 0.0;
		break;
	}

	if (action == 0) return true;
	btRigidBody* uvcircle_RigidBody;
	btTransform trans;
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

	case 'R':
		GameReset();
		break;
	case 'L':
		PhysicsSimulation::getInstance()->SwitchDebugMode();
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

	return true;
}

bool BreakOut::OnGameTouchEvent(int eventId, int x, int y, int pointerId)
{
#ifdef ANDROID
	if (eventId == 0)
		RenderManager::getInstance()->SwitchBloomMode();
#endif
	return false;
}

bool BreakOut::OnGameZoomCamera(double xoffset, double yoffset)
{
	return false;
}

void BreakOut::GameReset()
{
	for (int i = 0; i < 20; i++)
	{
		listcube[i].SetVisible(true);
		listcube[i].GetRigidBody()->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
	}

	uvcircle.ClearForcesPhysics();

	uvcircle.Translate(glm::vec3(0.0f, 0.0f, -10.0f));
	uvcircle.GetRigidBody()->forceActivationState(ACTIVE_TAG);
	uvcircle.GetRigidBody()->applyCentralForce(btVector3(1000., 0., -5000.));
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