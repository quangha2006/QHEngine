#ifndef __MESH_H__
#define __MESH_H__

#include <assimp/Importer.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "QHTexture.h"
#include "Shader.h"
#include "RenderTarget.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

struct Vertex {
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	glm::vec4 weight;
	glm::vec4 id;
	Vertex()
	{
		Position = glm::vec3(0.0f, 0.0f, 0.0f);
		Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		Normal = glm::vec3(1.0f, 1.0f, 1.0f);
		TexCoords = glm::vec2(0.0f, 0.0f);
		Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
		Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
		weight = glm::vec4(0.0f);
		id = glm::vec4(-1.0f);
	}
};
struct Material {
	glm::vec3 ambient; 
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	float transparent;
};

struct BoneInfo
{
	glm::mat4 BoneOffset;
	glm::mat4 FinalTransformation;

	BoneInfo()
	{
		BoneOffset = glm::mat4(0);
		FinalTransformation = glm::mat4(0);
	}
};
class Mesh {
private:
	std::string mMeshName;
	bool mHasNormals;
	bool mHasBone;
	bool mIsDrawPolygon;

	GLuint mIndices_index;
	GLuint mIndices_size;

	GLuint mMaterial_Id;

	Vertex *mVertex;
	GLuint mNumVertex;

	GLuint *mIndices;
	GLuint mNumIndices;
	glm::mat4 mLocalTransformation;
public:
	void SetUseLighting(bool isuse);
	void SetDrawPolygon(bool isdrawpolygon);
	void SetVertex(Vertex * vertex, GLuint numvertex);
	void SetIndices(GLuint *indices, GLuint numindices);
	void SetLocalTransformation(glm::mat4 transformation);
	Vertex *GetVertex(GLuint &numvertex);
	GLuint *GetIndices(GLuint &numindices);
	const std::string &GetName();
	GLuint GetIndicesIndex();
	GLuint GetIndicesSize();
	GLuint GetMaterialId();
	void Draw(RenderTargetType RT_Type
		, bool useCustomColor = false
		, const glm::vec3 &customColor = glm::vec3(0.0f, 0.0f, 0.0f));

	Mesh(GLuint index_begin
		,GLuint index_size
		,GLuint material_id
		,const std::string &meshname
		,bool hasnormals = false
		,bool hasbone = false);
	~Mesh();
};
#endif //!__MESH_H__