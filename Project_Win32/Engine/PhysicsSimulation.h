#ifndef __PHYSICSSIMULATION_H__
#define __PHYSICSSIMULATION_H__

#include "stdafx.h"
#include "btBulletDynamicsCommon.h"
#include "GLDebugDrawer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"

struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
	bool m_connected;
	btScalar m_margin;
	MyContactResultCallback() : m_connected(false), m_margin(0.05f)
	{
	}
	virtual btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
	{
		if (cp.getDistance() <= m_margin)
			m_connected = true;
		return 1.f;
	}
};

class PhysicsSimulation
{
private:
	static PhysicsSimulation* instance;

	btDefaultCollisionConfiguration* mCollisionConfiguration;
	btCollisionDispatcher* mDispatcher;
	btBroadphaseInterface* mOverlappingPairCache;
	btSequentialImpulseConstraintSolver* mSolver;
	btDiscreteDynamicsWorld* mDynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> mCollisionShapes;

	// debug
	int mDebugDrawModes;
	GLDebugDrawer debugDraw;

public:
	void initPhysics();
	void exitPhysics();
	void updatePhysics();
	void RenderPhysicsDebug();
	void PhysicsStepCollision(btCollisionObject* objA, btCollisionObject* objB, MyContactResultCallback &result);
	void SetGravity(btVector3 gravity);
	/**
		DBG_NoDebug = 0,
		DBG_DrawWireframe = 1,
		DBG_DrawAabb = 2,
		DBG_DrawFeaturesText = 4,
		DBG_DrawContactPoints = 8,
		DBG_NoDeactivation = 16,
		DBG_NoHelpText = 32,
		DBG_DrawText = 64,
		DBG_ProfileTimings = 128,
		DBG_EnableSatComparison = 256,
		DBG_DisableBulletLCP = 512,
		DBG_EnableCCD = 1024,
		DBG_DrawConstraints = (1 << 11),
		DBG_DrawConstraintLimits = (1 << 12),
		DBG_FastWireframe = (1 << 13),
		DBG_DrawNormals = (1 << 14),
		DBG_DrawFrames = (1 << 15),
		DBG_MAX_DEBUG_DRAW_MODE
	**/
	void SetDebugMode(int debugMode);
	int SwitchDebugMode();
	btRigidBody* createBoxShape(float mass, glm::vec3 pos, glm::vec3 rotate, float angle, glm::vec3 boxshape);
	btRigidBody* createSphereShape(float mass, glm::vec3 pos, glm::vec3 rotate, float angle, float radius);
	btRigidBody* registerShape(const Vertex* vertices, unsigned  int numvertice, glm::vec3 pos, glm::vec3 rotate, float angle);
	static PhysicsSimulation* getInstance();
	PhysicsSimulation();
	~PhysicsSimulation();
};
#endif // !__PHYSICSSIMULATION_H__