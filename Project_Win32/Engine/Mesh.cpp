#include "stdafx.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "Debugging.h"
#include "RenderManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include "Globals.h"


void Mesh::Draw(RenderMode mode, bool useCustomColor, const glm::vec3 &customColor)
{
	if (useCustomColor)
	{
		ShaderSet::setBool("useTexture", false);
		ShaderSet::setVec3("material_color_diffuse", customColor);
	}

	if (mIsDrawPolygon)
		QHEngine::DrawElements(GL_LINE_LOOP, mIndices_size, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * mIndices_index));
	else
		QHEngine::DrawElements(GL_TRIANGLES, mIndices_size, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * mIndices_index));

	CheckGLError(mMeshName.c_str());
}

void Mesh::SetUseLighting(bool isuse)
{
	mHasNormals = isuse;
}

void Mesh::SetDrawPolygon(bool isdrawpolygon)
{
	mIsDrawPolygon = isdrawpolygon;
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

Mesh::Mesh(GLuint indices_index
	, GLuint indices_size
	, GLuint material_id
	, const string &meshname
	, bool hasnormals
	, bool hasbone)
	: mIsDrawPolygon(false)
	, mIndices_index(indices_index)
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