#pragma once
#include <assimp/Importer.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QHTexture.h>
#include <Shader.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
using namespace std;
enum RenderMode
{
	RenderMode_Depth,
	RenderMode_Sence
};
/**
glm::vec3 Position;		// position
glm::vec3 Normal;		// normal
glm::vec2 TexCoords;	// texCoords
glm::vec3 Tangent;		// tangent
glm::vec3 Bitangent;	// bitangent
**/
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	glm::vec4 weight;
	glm::vec4 id;
	Vertex()
	{
		weight = glm::vec4(0.0f);
		id = glm::vec4(0.0f);
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
	unsigned int mVBO, mEBO;
	string mMeshName;
	bool mHasNormals;
	bool mHasBone;
	bool mIsDrawPolygon;
	glm::mat4 mTransform;
	/*  Mesh Data  */
	vector<Vertex> mVertices;
	vector<GLuint> mIndices;
	vector<Texture> mTextures;
	Material mMaterial;
	GLuint mTexture;

	void setupMesh();
public:
	void DeleteBuffer();
	void SetUseLighting(bool isuse);
	void SetDrawPolygon(bool isdrawpolygon);
	int GetNumVertex();
	std::string GetName();

	void Draw(RenderMode mode
		, bool isEnableAlpha = false
		, bool useCustomColor = false
		, const glm::vec3 &customColor = glm::vec3(0.0f, 0.0f, 0.0f));

	Mesh(const vector<Vertex> &vertices
		,const vector<GLuint> &indices
		,const vector<Texture> &textures
		,const Material &material
		,const string &meshname
		,const glm::mat4 &nodeTransformation
		,bool hasnormals = false
		,bool hasbone = false);
	~Mesh();
};