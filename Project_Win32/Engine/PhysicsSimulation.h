#pragma once
#include "btBulletDynamicsCommon.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class PhysicsSimulation
{
private:
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
public:
	void initPhysics();
	void exitPhysics();
	void updatePhysics();
	void createRigidBody(float mass, glm::mat4 transform, glm::vec3 boxshape);
	PhysicsSimulation();
	~PhysicsSimulation();
};

