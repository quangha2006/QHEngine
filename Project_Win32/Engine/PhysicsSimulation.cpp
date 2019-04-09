#include "PhysicsSimulation.h"
#include "ShaderManager.h"
#include "Camera.h"

PhysicsSimulation * PhysicsSimulation::instance = NULL;
float *GenVerticeData(const btVector3& halfExtents);

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

	initDebugPhysics();
}

void PhysicsSimulation::initDebugPhysics()
{
	glBindVertexArray(0);
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, 12 * 6 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CheckGLError("initDebugPhysics");
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
	if (!misRenderDebug)
		return;

	ShaderManager::getInstance()->setUseProgram("debugPhysics");

	if (misEnableDepthTestDebug)
		glEnable(GL_DEPTH_TEST);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	for (int j = mDynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
	{
		btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[j];
		btRigidBody* body = btRigidBody::upcast(obj);
		btTransform trans;
		if (body)
		{
			trans = body->getWorldTransform();
		}
		else
		{
			trans = obj->getWorldTransform();
		}

		float trans_x = float(trans.getOrigin().getX());
		float trans_y = float(trans.getOrigin().getY());
		float trans_z = float(trans.getOrigin().getZ());

		btVector3 halfextents(btScalar(1.0), btScalar(1.0), btScalar(1.0));

		int type = obj->getCollisionShape()->getShapeType();
		if (type == BOX_SHAPE_PROXYTYPE)
		{
			btBoxShape* colShape = static_cast<btBoxShape*> (obj->getCollisionShape());
			halfextents = colShape->getHalfExtentsWithoutMargin();
		}
		else if (type == SPHERE_SHAPE_PROXYTYPE)
		{
			btSphereShape* colShape = static_cast<btSphereShape*> (obj->getCollisionShape());
			btScalar radius = colShape->getRadius();
			halfextents = btVector3(radius, radius, radius);
		}

		float *vertices = GenVerticeData(halfextents);

		glBufferSubData(GL_ARRAY_BUFFER, 0, 12 * 6 * sizeof(float), vertices);

		btQuaternion rotation = trans.getRotation();
		float x = float(trans.getOrigin().getX());
		float y = float(trans.getOrigin().getY());
		float z = float(trans.getOrigin().getZ());

		btScalar angle = rotation.getAngle();
		btScalar ro_x = rotation.getAxis().getX();
		btScalar ro_y = rotation.getAxis().getY();
		btScalar ro_z = rotation.getAxis().getZ();


		glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(trans_x, trans_y, trans_z));

		model = rotate(model, rotation.getAngle(), vec3(ro_x, ro_y, ro_z));

		glm::mat4 WorldViewProjectionMatrix = Camera::getInstance()->WorldViewProjectionMatrix * model;
		ShaderSet::setMat4("WorldViewProjectionMatrix", WorldViewProjectionMatrix);
		glDrawArrays(GL_LINES, 0, 24);

		delete[]vertices;
	}

	
	glBindVertexArray(0);

	if (misEnableDepthTestDebug)
		glDisable(GL_DEPTH_TEST);

	CheckGLError("RenderPhysicsDebug");
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

	btCollisionShape* colShape = new btBoxShape(btVector3(btScalar(boxshape.x), btScalar(boxshape.y), btScalar(boxshape.z)));
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
	btCollisionShape* colShape = new btSphereShape(btScalar(radius));
	//btCompoundShape* colShape = new btCompoundShape();
	//colShape->addChildShape(btTransform::getIdentity(), childShape);

	mCollisionShapes.push_back(colShape);

	// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	startTransform.setRotation(btQuaternion(btVector3(1, 1, 1), SIMD_PI / 10.));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(btScalar(pos.x), btScalar(pos.y), btScalar(pos.z)));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	mDynamicsWorld->addRigidBody(body);

	return body;
}
void PhysicsSimulation::SwitchDebugMode()
{
	if (misRenderDebug && misEnableDepthTestDebug)
	{
		misRenderDebug = !misRenderDebug;
		misEnableDepthTestDebug = !misEnableDepthTestDebug;
		return;
	}
	if (misRenderDebug && !misEnableDepthTestDebug)
	{
		misEnableDepthTestDebug = !misEnableDepthTestDebug;
		return;
	}
	if (!misRenderDebug)
	{
		misRenderDebug = !misRenderDebug;
		misEnableDepthTestDebug = false;
		return;
	}
}

PhysicsSimulation * PhysicsSimulation::getInstance()
{
	if (instance == NULL)
		instance = new PhysicsSimulation();
	return instance;
}

PhysicsSimulation::PhysicsSimulation()
{
	misRenderDebug = false;
	misEnableDepthTestDebug = true;
}


PhysicsSimulation::~PhysicsSimulation()
{
	glDeleteBuffers(1, &quadVBO);
}

float *GenVerticeData(const btVector3& halfExtents)
{
	float x = halfExtents.getX();
	float y = halfExtents.getY();
	float z = halfExtents.getZ();

	// -x, y, z, //1
	//	x, y, z, //2
	//	x,-y, z, //3
	// -x, -y, z, //4
	// -x,  y,-z, //5
	//	x, y, -z, //6
	//	x, -y, -z, //7
	// -x, -y, -z, //8
	float *vertices = new float[12 * 6]{
		-x, y, z,
		 x, y, z,
		 x, y, z,
		 x,-y, z,
		 x,-y, z,
		-x,-y, z,
		-x,-y, z,
		-x, y, z,
		-x, y, z,
		-x, y,-z,
		 x, y, z,
		 x, y,-z,
		 x,-y, z,
		 x,-y,-z,
		-x,-y, z,
		-x,-y,-z,
		-x, y,-z,
		 x, y,-z,
		 x, y,-z,
		 x,-y,-z,
		 x,-y,-z,
		-x,-y,-z,
		-x,-y,-z,
		-x, y,-z
	};
	return vertices;
}