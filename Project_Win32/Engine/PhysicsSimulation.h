#pragma once
#include "stdafx.h"
#include "btBulletDynamicsCommon.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
	bool m_connected;
	btScalar m_margin;
	MyContactResultCallback() : m_connected(false), m_margin(0.05)
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
	GLuint quadVAO, quadVBO;
public:
	void initPhysics();
	void initDebugPhysics();
	void exitPhysics();
	void updatePhysics();
	void RenderPhysicsDebug();
	void PhysicsStepCollision(btCollisionObject* objA, btCollisionObject* objB, MyContactResultCallback &result);
	void SetGravity(btVector3 gravity);
	btRigidBody* createBoxShape(float mass, glm::vec3 pos, glm::vec3 rotate, float angle, glm::vec3 boxshape);
	btRigidBody* createSphereShape(float mass, glm::vec3 pos, glm::vec3 rotate, float angle, float radius);
	static PhysicsSimulation* getInstance();
	PhysicsSimulation();
	~PhysicsSimulation();
};

