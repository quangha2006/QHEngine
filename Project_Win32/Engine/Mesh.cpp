#include "stdafx.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "Debugging.h"
#include "RenderManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include "Globals.h"

void Mesh::Draw(RenderTargetType RT_Type, bool isDrawWireFrame, bool useCustomColor, const glm::vec3 &customColor)
{
	if (useCustomColor)
	{
		ShaderSet::setBool("useTexture", false);
		ShaderSet::setVec3("material_color_diffuse", customColor);
	}
	
	//ShaderSet::setMat4("localTransform",mLocalTransformation);

	if (isDrawWireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	QHEngine::DrawElements(GL_TRIANGLES, mIndices_size, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * mIndices_index));

	if (isDrawWireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	CheckGLError(mMeshName.c_str());
}

void Mesh::SetUseLighting(bool isuse)
{
	mHasNormals = isuse;
}

void Mesh::SetVertex(Vertex * vertex, GLuint numvertex)
{
	mVertex = vertex;
	mNumVertex = numvertex;
}

void Mesh::SetIndices(GLuint * indices, GLuint numindices)
{
	mIndices = indices;
	mNumIndices = numindices;
}

void Mesh::SetLocalTransformation(glm::mat4 transformation)
{
	mLocalTransformation = transformation;
}

Vertex * Mesh::GetVertex(GLuint & numvertex)
{
	numvertex = mNumVertex;
	return mVertex;
}

GLuint * Mesh::GetIndices(GLuint & numindices)
{
	numindices = mNumIndices;
	return mIndices;
}

const std::string &Mesh::GetName()
{
	return mMeshName;
}

GLuint Mesh::GetIndicesIndex()
{
	return mIndices_index;
}

GLuint Mesh::GetIndicesSize()
{
	return mIndices_size;
}

GLuint Mesh::GetMaterialId()
{
	return mMaterial_Id;
}

void Mesh::SetMaterialId(GLuint newid)
{
	mMaterial_Id = newid;
}

void Mesh::registerShapeTriangle(float mass, bool isOptimize)
{
	isDynamic = (mass != 0.f);
	mRigidBody = PhysicsSimulation::getInstance()->createConvexTriangleMeshShape(mass, mVertex, mNumVertex, mIndices, mNumIndices, glm::vec3(0.0f), glm::vec3(0.0f), 0.0f, glm::vec3(1.0f));
	if (isDynamic)
		mRigidBody->setActivationState(DISABLE_DEACTIVATION);
	mRigidBody->setFriction(0.0);
	mRigidBody->setRestitution(1.);
}

Mesh::Mesh(GLuint indices_index
	, GLuint indices_size
	, GLuint material_id
	, const std::string &meshname
	, bool hasnormals
	, bool hasbone)
	: mIndices_index(indices_index)
	, mIndices_size(indices_size)
	, mMaterial_Id(material_id)
	, mMeshName(meshname)
	, mHasNormals(hasnormals)
	, mHasBone(hasbone)
	, mVertex(nullptr)
	, mNumVertex(0)
	, mIndices(nullptr)
	, mNumIndices(0)
{

}
Mesh::~Mesh()
{
	if (mVertex != nullptr)
		delete[] mVertex;

	if (mIndices != nullptr)
		delete[] mIndices;
}