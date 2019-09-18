#include "PhysicsSimulation.h"
#include "ShaderManager.h"
#include "Camera.h"

PhysicsSimulation * PhysicsSimulation::instance = NULL;

void PhysicsSimulation::initPhysics()
{
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	mCollisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	mOverlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mSolver = new btSequentialImpulseConstraintSolver;

	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);

	mDynamicsWorld->setGravity(btVector3(0, -100.0, 0));

	//debugDraw.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	mDynamicsWorld->setDebugDrawer(&debugDraw);
}

void PhysicsSimulation::exitPhysics()
{
	for (int i = mDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		mDynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}
	//delete collision shapes
	for (int j = 0; j < mCollisionShapes.size(); j++)
	{
		btCollisionShape* shape = mCollisionShapes[j];
		mCollisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete mDynamicsWorld;

	//delete solver
	delete mSolver;

	//delete broadphase
	delete mOverlappingPairCache;

	//delete dispatcher
	delete mDispatcher;

	delete mCollisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	mCollisionShapes.clear();
}

void PhysicsSimulation::updatePhysics()
{
	mDynamicsWorld->stepSimulation(1.f / 60.f, 10);
}

void PhysicsSimulation::RenderPhysicsDebug()
{
	mDynamicsWorld->debugDrawWorld();
}

void PhysicsSimulation::PhysicsStepCollision(btCollisionObject* objA, btCollisionObject* objB, MyContactResultCallback &result)
{
	mDynamicsWorld->contactPairTest(objA, objB, result);
}

void PhysicsSimulation::SetGravity(btVector3 gravity)
{
	mDynamicsWorld->setGravity(gravity);
}

btRigidBody* PhysicsSimulation::createBoxShape(float mass, glm::vec3 pos, glm::vec3 rotate, float angle, glm::vec3 boxshape)
{
	//create a dynamic rigidbody

	btCollisionShape* colShape = new btBoxShape(btVector3(boxshape.x, boxshape.y, boxshape.z));
	mCollisionShapes.push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	//rigidbody is dynamic if and only if mass is non zero, otherwise static

	bool isDynamic = (mass != 0.f);
	
	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
	{
		colShape->calculateLocalInertia(mass, localInertia);
	}

	startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

	if (angle != 0.0f)
	{
		btQuaternion startQuater(btVector3(rotate.x, rotate.y, rotate.z), glm::radians(angle));
		startTransform.setRotation(startQuater);
	}

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	mDynamicsWorld->addRigidBody(body);

	return body;
}

btRigidBody * PhysicsSimulation::createSphereShape(float mass, glm::vec3 pos, glm::vec3 rotate, float angle, float radius)
{
	btCollisionShape* colShape = new btSphereShape(radius);
	//btCompoundShape* colShape = new btCompoundShape();
	//colShape->addChildShape(btTransform::getIdentity(), childShape);

	mCollisionShapes.push_back(colShape);

	// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	if (angle != 0.0f)
	{
		btQuaternion startQuater(btVector3(rotate.x, rotate.y, rotate.z), glm::radians(angle));
		startTransform.setRotation(startQuater);
	}

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	mDynamicsWorld->addRigidBody(body);

	return body;
}
btRigidBody * PhysicsSimulation::registerShape(float mass, const Vertex* vertices, unsigned int numvertice, glm::vec3 pos, glm::vec3 rotate, float angle, glm::vec3 scale, bool isOptimize)
{
	if (!vertices || numvertice <= 0) return NULL;
	btScalar* verticesData = new btScalar[numvertice * 3];
	unsigned int count = 0;
	while (count < numvertice)
	{
		unsigned int index = count * 3;
		verticesData[index]		= vertices[count].Position.x * scale.x;
		verticesData[index + 1] = vertices[count].Position.y * scale.y;
		verticesData[index + 2] = vertices[count].Position.z * scale.z;
		++count;
	}
	btConvexHullShape* shape = new btConvexHullShape(verticesData, numvertice, sizeof(btScalar) * 3);

	if (isOptimize)
		shape->optimizeConvexHull();

	mCollisionShapes.push_back(shape);

	btTransform startTransform;
	startTransform.setIdentity();
	

	bool isDynamic = (mass != 0.f);
	
	btVector3 localInertia(0, 0, 0);

	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	btVector3 position(pos.x, pos.y, pos.z);
	startTransform.setOrigin(position);

	if (angle != 0.0f)
	{
		btQuaternion startQuater(btVector3(rotate.x, rotate.y, rotate.z), glm::radians(angle));
		startTransform.setRotation(startQuater);
	}

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	mDynamicsWorld->addRigidBody(body);

	delete[] verticesData;
	return body;
}
btRigidBody * PhysicsSimulation::registerShapeTriangle(float mass, const Vertex * vertices, unsigned int numvertice, GLuint * indices, GLuint numIndices, glm::vec3 pos, glm::vec3 rotate, float angle, glm::vec3 scale)
{
	if (!vertices || numvertice <= 0 || !indices || numIndices <= 0) return NULL;

	btTriangleMesh* meshInterface = new btTriangleMesh();

	for (int i = 0; i < numIndices / 3; i++)
	{
		glm::vec3 v0 = vertices[indices[i * 3]].Position * scale;
		glm::vec3 v1 = vertices[indices[i * 3 + 1]].Position * scale;
		glm::vec3 v2 = vertices[indices[i * 3 + 2]].Position * scale;

		meshInterface->addTriangle(btVector3(v0.x, v0.y, v0.z),
			btVector3(v1.x, v1.y, v1.z),
			btVector3(v2.x, v2.y, v2.z));
	}

	btCompoundShape* compound = new btCompoundShape();

	btConvexShape* hull = new btConvexTriangleMeshShape(meshInterface);

	mCollisionShapes.push_back(hull);

	btTransform startTransform;
	startTransform.setIdentity();

	if (angle != 0.0f)
	{
		btQuaternion startQuater(btVector3(rotate.x, rotate.y, rotate.z), glm::radians(angle));
		startTransform.setRotation(startQuater);
	}

	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);

	if (isDynamic)
		hull->calculateLocalInertia(mass, localInertia);

	btVector3 position(pos.x, pos.y, pos.z);
	startTransform.setOrigin(position);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, hull, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	mDynamicsWorld->addRigidBody(body);

	return body;
}
void PhysicsSimulation::SetDebugMode(int debugMode)
{
	mDebugDrawModes = debugMode;
	debugDraw.setDebugMode(debugMode);
}

int PhysicsSimulation::SwitchDebugMode()
{
	int currentDBG = mDebugDrawModes;
	switch (currentDBG)
	{
	case btIDebugDraw::DBG_NoDebug:
		mDebugDrawModes = btIDebugDraw::DBG_DrawWireframe;
		break;
	case btIDebugDraw::DBG_DrawWireframe:
		mDebugDrawModes = btIDebugDraw::DBG_DrawWireframe;
		mDebugDrawModes |= btIDebugDraw::DBG_DrawAabb;
		break;
	default:
		mDebugDrawModes = btIDebugDraw::DBG_NoDebug;
	}
	debugDraw.setDebugMode(mDebugDrawModes);

	return mDebugDrawModes;
}

PhysicsSimulation * PhysicsSimulation::getInstance()
{
	if (instance == NULL)
		instance = new PhysicsSimulation();
	return instance;
}

PhysicsSimulation::PhysicsSimulation()
	: mDebugDrawModes(0)
{
}


PhysicsSimulation::~PhysicsSimulation()
{
}