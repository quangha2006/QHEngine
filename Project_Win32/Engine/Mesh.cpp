#include "stdafx.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "Debugging.h"
#include "RenderManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include "Globals.h"


void Mesh::Draw(RenderMode mode, bool isEnableAlpha, bool useCustomColor, const glm::vec3 &customColor)
{
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	bool hasmaterial_texture_diffuse1 = false;
	ShaderSet::setBool("useNormalMap", false);
	ShaderSet::setBool("enableAlpha", isEnableAlpha);
	ShaderSet::setMat4("Transform", mTransform);
	/*if (hasBone)
		ShaderSet::setBool("useAnim", true);
	else
		ShaderSet::setBool("useAnim", false);*/
	unsigned int i = 0;
	for (; i < mTextures.size(); i++)
	{
		if (!isEnableAlpha && mode == RenderMode_Depth) break;

		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
										  // retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name;
		aiTextureType texturetype = mTextures[i].type;
		if (texturetype == aiTextureType_DIFFUSE)
		{
			name = "texture_diffuse";
			ss << diffuseNr++; // transfer unsigned int to stream
			hasmaterial_texture_diffuse1 = true;
		}
		else if (texturetype == aiTextureType_SPECULAR)
		{
			name = "texture_specular";
			ss << specularNr++; // transfer unsigned int to stream
			hasmaterial_texture_diffuse1 = true;
		}
		else if (texturetype == aiTextureType_NORMALS || texturetype == aiTextureType_HEIGHT)
		{
			name = "texture_normal";
			ss << normalNr++; // transfer unsigned int to stream
			ShaderSet::setBool("useNormalMap", true);
		}
		number = ss.str();
		// now set the sampler to the correct texture unit
		string Material = "material_";
		string full_name = Material + name + number;
		//int temp = glGetUniformLocation(ShaderSet::GetCurrentProgram(), full_name.c_str());

		ShaderSet::setInt(full_name.c_str(), i);

		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
	}
	if (mode == RenderMode_Sence)
	{
		GLuint depthmap = RenderManager::getInstance()->GetDepthMapId();
		if (depthmap != 0)
		{
			ShaderSet::setBool("useShadowMap", true);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, depthmap);
			ShaderSet::setInt("shadowMap", i);
		}
		else
			ShaderSet::setBool("useShadowMap", false);
	}

	ShaderSet::setFloat("material_transparent", mMaterial.transparent);
	ShaderSet::setFloat("material_shininess", mMaterial.shininess);
	ShaderSet::setVec3("material_color_ambient", mMaterial.ambient);
	ShaderSet::setVec3("material_color_diffuse", mMaterial.diffuse);
	ShaderSet::setVec3("material_color_specular", mMaterial.specular);
	if (mMaterial.shininess < 0.001f || !mHasNormals)
		ShaderSet::setBool("uselighting", false);

	if (!isEnableAlpha && mode == RenderMode_Depth)
		ShaderSet::setBool("useTexture", false);
	else if (hasmaterial_texture_diffuse1)
		ShaderSet::setBool("useTexture", true);
	else
		ShaderSet::setBool("useTexture", false);

	if (useCustomColor)
	{
		ShaderSet::setBool("useTexture", false);
		ShaderSet::setVec3("material_color_diffuse", customColor);
	}

	if (mIsDrawPolygon)
		QHEngine::DrawElements(GL_LINE_LOOP, mIndices_size, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mIndices_index));
	else
		QHEngine::DrawElements(GL_TRIANGLES, mIndices_size, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mIndices_index));

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

const std::string &Mesh::GetName()
{
	return mMeshName;
}

unsigned int Mesh::GetIndicesIndex()
{
	return mIndices_index;
}

unsigned int Mesh::GetIndicesSize()
{
	return mIndices_size;
}

Mesh::Mesh(unsigned int indices_index
	, unsigned int indices_size
	, const vector<Texture> &textures
	, const Material &meterial
	, const string &meshname
	, const glm::mat4 &nodeTransformation
	, bool hasnormals
	, bool hasbone)
	: mIsDrawPolygon(false)
	, mIndices_index(indices_index)
	, mIndices_size(indices_size)
	, mTextures(textures)
	, mMaterial(meterial)
	, mMeshName(meshname)
	, mHasNormals(hasnormals)
	, mHasBone(hasbone)
	, mTransform(nodeTransformation)
{

}
Mesh::~Mesh()
{

}