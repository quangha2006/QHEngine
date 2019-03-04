#pragma once
#include <AppBase.h>
#include <AppContext.h>
#include <SkyBox.h>
#include <Model.h>
#include "btBulletDynamicsCommon.h"

class BreakOut : public AppBase
{
private:
	Model m_Streetenvironment, cube, uvcircle;
	//Model listcube[20];
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
public:
	void Update();
	void Init();
	void GetRequireScreenSize(int32_t &width, int32_t &height);
	void OnGameKeyPressed(int key, int scancode, int action, int mods);
	BreakOut();
	~BreakOut();
};

