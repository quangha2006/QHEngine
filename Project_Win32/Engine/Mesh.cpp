#include "stdafx.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "Debugging.h"
#include "RenderManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include "Globals.h"

void Mesh::setupMesh()
{

	// create buffers/arrays
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), &mIndices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Mesh::Draw(RenderMode mode, bool isEnableAlpha, bool useCustomColor, glm::vec3 customColor)
{
	Shader * modelShader = ShaderManager::getInstance()->GetCurrentShader();

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	if (modelShader->position_Attribute != -1)
	{
		glEnableVertexAttribArray(modelShader->position_Attribute);
		glVertexAttribPointer(modelShader->position_Attribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	}
	// vertex texture coords
	if (modelShader->TexCoord_Attribute != -1)
	{
		glEnableVertexAttribArray(modelShader->TexCoord_Attribute);
		glVertexAttribPointer(modelShader->TexCoord_Attribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	}
	// s_vWeights
	if (modelShader->Weights_Attribute != -1)
	{
		glEnableVertexAttribArray(modelShader->Weights_Attribute);
		glVertexAttribPointer(modelShader->Weights_Attribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weight));
	}
	// s_vIDs;
	if (modelShader->IDs_Attribute != -1)
	{
		glEnableVertexAttribArray(modelShader->IDs_Attribute);
		//glVertexAttribIPointer(modelShader->IDs_Attribute, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, id)); //for ivec4
		glVertexAttribPointer(modelShader->IDs_Attribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, id));
	}
	// vertex normals
	if (modelShader->normal_Attribute != -1)
	{
		glEnableVertexAttribArray(modelShader->normal_Attribute);
		glVertexAttribPointer(modelShader->normal_Attribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	}
	// vertex tangent
	if (modelShader->Tangent_Attribute != -1)
	{
		glEnableVertexAttribArray(modelShader->Tangent_Attribute);
		glVertexAttribPointer(modelShader->Tangent_Attribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	}
	// vertex bitangent
	if (modelShader->Bitangent_Attribute != -1)
	{
		glEnableVertexAttribArray(modelShader->Bitangent_Attribute);
		glVertexAttribPointer(modelShader->Bitangent_Attribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	}


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
	if (mode != RenderMode_Depth)
	{
		GLuint depthmap = RenderManager::getInstance()->GetDepthMapId();
		if (depthmap != -1)
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
		QHEngine::DrawElements(GL_LINE_LOOP, mIndices.size(), GL_UNSIGNED_INT, (void*)0);
	else
		QHEngine::DrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CheckGLError(mMeshName.c_str());
}

void Mesh::DeleteBuffer()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);
}

void Mesh::SetUseLighting(bool isuse)
{
	mHasNormals = isuse;
}

void Mesh::SetDrawPolygon(bool isdrawpolygon)
{
	mIsDrawPolygon = isdrawpolygon;
}

int Mesh::GetNumVertex()
{
	return mVertices.size();
}

std::string Mesh::GetName()
{
	return mMeshName;
}

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, Material meterial, string meshname, glm::mat4 nodeTransformation, bool hasnormals, bool hasbone)
	: mIsDrawPolygon(false)
	, mVertices(vertices)
	, mIndices(indices)
	, mTextures(textures)
	, mMaterial(meterial)
	, mMeshName(meshname)
	, mHasNormals(hasnormals)
	, mHasBone(hasbone)
	, mTransform(nodeTransformation)
{
	setupMesh();
}
Mesh::~Mesh()
{
}