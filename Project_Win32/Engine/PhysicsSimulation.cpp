#include "PhysicsSimulation.h"
#include "Camera.h"
////////////
// Test
#include "landscapeData.h"

int LandscapeVtxCount[] = {
	Landscape01VtxCount,
	Landscape02VtxCount,
	Landscape03VtxCount,
	Landscape04VtxCount,
	Landscape05VtxCount,
	Landscape06VtxCount,
	Landscape07VtxCount,
	Landscape08VtxCount,
};

int LandscapeIdxCount[] = {
	Landscape01IdxCount,
	Landscape02IdxCount,
	Landscape03IdxCount,
	Landscape04IdxCount,
	Landscape05IdxCount,
	Landscape06IdxCount,
	Landscape07IdxCount,
	Landscape08IdxCount,
};

btScalar* LandscapeVtx[] = {
	Landscape01Vtx,
	Landscape02Vtx,
	Landscape03Vtx,
	Landscape04Vtx,
	Landscape05Vtx,
	Landscape06Vtx,
	Landscape07Vtx,
	Landscape08Vtx,
};

btScalar* LandscapeNml[] = {
	Landscape01Nml,
	Landscape02Nml,
	Landscape03Nml,
	Landscape04Nml,
	Landscape05Nml,
	Landscape06Nml,
	Landscape07Nml,
	Landscape08Nml,
};

btScalar* LandscapeTex[] = {
	Landscape01Tex,
	Landscape02Tex,
	Landscape03Tex,
	Landscape04Tex,
	Landscape05Tex,
	Landscape06Tex,
	Landscape07Tex,
	Landscape08Tex,
};

unsigned short* LandscapeIdx[] = {
	Landscape01Idx,
	Landscape02Idx,
	Landscape03Idx,
	Landscape04Idx,
	Landscape05Idx,
	Landscape06Idx,
	Landscape07Idx,
	Landscape08Idx,
};
// End Test
///////////
PhysicsSimulation * PhysicsSimulation::instance = NULL;

btRigidBody * PhysicsSimulation::createRigidBody(float mass, const btTransform & startTransform, btCollisionShape * shape)
{
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);

	body->setUserIndex(-1);
	mDynamicsWorld->addRigidBody(body);
	return body;
}

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

	mDynamicsWorld->setGravity(btVector3(0, -10.0, 0));

	debugDraw.setDebugMode(btIDebugDraw::DBG_DrawFeaturesText);
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

void PhysicsSimulation::updatePhysics(float detatime)
{
	mDynamicsWorld->stepSimulation(detatime);
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

btRigidBody* PhysicsSimulation::createBoxShape(float mass, glm::vec3 pos, glm::vec3 rotate, float angle, glm::vec3 boxshape, glm::vec3 localScaling)
{
	//create a dynamic rigidbody

	btCollisionShape* colShape = new btBoxShape(btVector3(boxshape.x, boxshape.y, boxshape.z));
	colShape->setLocalScaling(btVector3(localScaling.x, localScaling.y, localScaling.z));
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

	return createRigidBody(mass, startTransform, colShape);
	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

	//btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	//btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	//btRigidBody* body = new btRigidBody(rbInfo);

	//mDynamicsWorld->addRigidBody(body);

	//return body;
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
	return createRigidBody(mass, startTransform, colShape);
	/*btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	mDynamicsWorld->addRigidBody(body);

	return body;*/
}
btRigidBody * PhysicsSimulation::createConvexHullShape(float mass, const Vertex* vertices, unsigned int numvertice, glm::vec3 pos, glm::vec3 rotate, float angle, glm::vec3 scale, bool isOptimize)
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

	//btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	
	//btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = createRigidBody(mass, startTransform, shape);

	delete[] verticesData;
	return body;
}
btRigidBody * PhysicsSimulation::createConvexTriangleMeshShape(float mass, const Vertex * vertices, unsigned int numvertice, GLuint * indices, GLuint numIndices, glm::vec3 pos, glm::vec3 rotate, float angle, glm::vec3 scale)
{
	if (!vertices || numvertice <= 0 || !indices || numIndices <= 0) return NULL;

	btTriangleMesh* meshInterface = new btTriangleMesh();

	for (unsigned int i = 0; i < numIndices / 3; i++)
	{
		glm::vec3 v0 = vertices[indices[i * 3]].Position * scale;
		glm::vec3 v1 = vertices[indices[i * 3 + 1]].Position * scale;
		glm::vec3 v2 = vertices[indices[i * 3 + 2]].Position * scale;

		meshInterface->addTriangle(btVector3(v0.x, v0.y, v0.z),
			btVector3(v1.x, v1.y, v1.z),
			btVector3(v2.x, v2.y, v2.z));
	}

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

	/*btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, hull, localInertia);*/
	btRigidBody* body = createRigidBody(mass, startTransform, hull);

	/*mDynamicsWorld->addRigidBody(body);*/

	return body;
}
btRigidBody * PhysicsSimulation::createTriangleMeshShape(float mass, const Vertex * vertices, unsigned int numvertice, GLuint * indices, GLuint numIndices, glm::vec3 pos, glm::vec3 rotate, float angle, glm::vec3 scale)
{
	btTransform trans;
	trans.setIdentity();

	float *LandscapeVtx = new float[numvertice * 3];

	for (unsigned int i = 0; i < numvertice; i++)
	{
		LandscapeVtx[i * 3] = vertices[i].Position.x;
		LandscapeVtx[i * 3 + 1] = vertices[i].Position.y;
		LandscapeVtx[i * 3 + 2] = vertices[i].Position.z;
	}

	//Convert Vertex to array

	btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray();
	btIndexedMesh part;

	part.m_vertexBase = (const unsigned char*)LandscapeVtx;
	part.m_vertexStride = sizeof(btScalar) * 3;
	part.m_numVertices = numvertice;
	part.m_triangleIndexBase = (const unsigned char*)indices;
	part.m_triangleIndexStride = sizeof(GLuint) * 3;
	part.m_numTriangles = numIndices / 3;
	part.m_indexType = PHY_INTEGER;
	meshInterface->addIndexedMesh(part, PHY_INTEGER);

	bool useQuantizedAabbCompression = true;
	btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(meshInterface, useQuantizedAabbCompression);
	btVector3 localInertia(0, 0, 0);

	trans.setOrigin(btVector3(pos.x, pos.y, pos.z));

	trimeshShape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

	btRigidBody* body = createRigidBody(mass, trans, trimeshShape);

	//delete[]LandscapeVtx;

	return body;
}
btRigidBody * PhysicsSimulation::createCapsuleShape(float mass, float radius, float height, glm::vec3 localScaling, glm::vec3 position, glm::vec3 positionOffset)
{
	float scale = 1.0f;
	btCapsuleShape* CapsulecolShape = new btCapsuleShape(radius , height - 2 * radius);
	//btCapsuleShape* CapsulecolShape = new btCapsuleShape(radius , height);
	float originHeight = height - 2 * radius;
	float ExspecHeight = height * localScaling.y - 2 * radius;

	CapsulecolShape->setLocalScaling(btVector3(localScaling.x, ExspecHeight / originHeight, localScaling.z));

	btCollisionShape* colShape = CapsulecolShape;

	btTransform offset;
	offset.setIdentity();
	offset.setOrigin(btVector3(positionOffset.x, positionOffset.y, positionOffset.z));

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(scale * btVector3(position.x, position.y, position.z));

	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);

	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	return createRigidBody(mass, transform, colShape);
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