#pragma once
#include "stdafx.h"
#include "btBulletDynamicsCommon.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class PhysicsSimulation
{
private:
	static PhysicsSimulation* instance;

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	// debug
	GLuint quadVAO, quadVBO;
public:
	void initPhysics();
	void initDebugPhysics();
	void exitPhysics();
	void updatePhysics();
	void RenderPhysicsDebug();
	btRigidBody* createBoxShape(float mass, glm::vec3 pos, glm::vec3 rotate, float angle, glm::vec3 boxshape);
	btRigidBody* createSphereShape(float mass, float radius, glm::vec3 rotate, float angle, glm::vec3 boxshape);
	static PhysicsSimulation* getInstance();
	PhysicsSimulation();
	~PhysicsSimulation();
};

