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
	//Shaderv2 * modelShader = ShaderManager::getInstance()->GetShader("basic");
	//if (modelShader == nullptr) return;

	// create buffers/arrays
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Mesh::Draw(RenderMode mode, bool useCustomColor, glm::vec3 customColor)
{
	Shaderv2 * modelShader = ShaderManager::getInstance()->GetCurrentShader();

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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
		//glVertexAttribIPointer(modelShader->IDs_Attribute, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, id)); //use for ivec4
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
	ShaderManager::getInstance()->setBool("useNormalMap", false);
	if (hasBone)
		ShaderManager::getInstance()->setBool("useAnim", true);
	else
		ShaderManager::getInstance()->setBool("useAnim", false);
	unsigned int i = 0;
	for (; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
										  // retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name;
		aiTextureType texturetype = textures[i].type;
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
			ShaderManager::getInstance()->setBool("useNormalMap", true);
		}
		number = ss.str();
		// now set the sampler to the correct texture unit
		string Material = "material_";
		string full_name = Material + name + number;
		//int temp = glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), full_name.c_str());

		ShaderManager::getInstance()->setInt(full_name.c_str(), i);

		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	if (mode != RenderMode_Depth)
	{
		GLuint depthmap = RenderManager::getInstance()->GetDepthMapId();
		if (depthmap != -1)
		{
			ShaderManager::getInstance()->setBool("useShadowMap", true);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, depthmap);
			ShaderManager::getInstance()->setInt("shadowMap", i);
		}
		else
			ShaderManager::getInstance()->setBool("useShadowMap", false);
	}

	ShaderManager::getInstance()->setFloat("material_transparent", material.transparent);
	ShaderManager::getInstance()->setFloat("material_shininess", material.shininess);
	ShaderManager::getInstance()->setVec3("material_color_ambient", material.ambient);
	ShaderManager::getInstance()->setVec3("material_color_diffuse", material.diffuse);
	ShaderManager::getInstance()->setVec3("material_color_specular", material.specular);
	if (material.shininess < 0.001f || !hasNormals)
		ShaderManager::getInstance()->setBool("uselighting", false);

	if (hasmaterial_texture_diffuse1)
		ShaderManager::getInstance()->setBool("useTexture", true);
	else
		ShaderManager::getInstance()->setBool("useTexture", false);

	if (useCustomColor)
	{
		ShaderManager::getInstance()->setBool("useTexture", false);
		ShaderManager::getInstance()->setVec3("material_color_diffuse", customColor);
	}
	CheckGLError("Draw mesh 1 ");
	if (isDrawPolygon)
		QHEngine::DrawElements(GL_LINE_LOOP, indices.size(), GL_UNSIGNED_INT, (void*)0);
	else
		QHEngine::DrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CheckGLError(meshName.c_str());
}

void Mesh::DeleteBuffer()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::SetUseLighting(bool isuse)
{
	this->hasNormals = isuse;
}

void Mesh::SetDrawPolygon(bool isdrawpolygon)
{
	isDrawPolygon = isdrawpolygon;
}

int Mesh::GetNumVertex()
{
	return vertices.size();
}

std::string Mesh::GetName()
{
	return meshName;
}

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, Material meterial, string meshname, bool hasnormals, bool hasbone)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->material = meterial;
	this->meshName = meshname;
	this->hasNormals = hasnormals;
	this->hasBone = hasbone;
	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	setupMesh();
	isDrawPolygon = false;
}
Mesh::~Mesh()
{
}