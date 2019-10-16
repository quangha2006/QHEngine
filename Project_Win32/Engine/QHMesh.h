#pragma once
#include "stdafx.h"
#include "QHVertex.h"
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include <map>
#include <vector>

class QHMesh
{
private:
	unsigned int mNumVertices;
	unsigned int mNumIndices;
	Vertex* mVerticesData;
	unsigned int* mIndicesData;
	unsigned int mMaterialIndex;
	GLuint mVBO, mEBO, mVAO, mInstancingBuff;

	bool mHasBones;
	bool mHasPos;
	bool mHasNormals;
	bool mHasTextureCoords0;
	bool mHasTangentsAndBitangents;
	bool mHasVertexColors0;
	std::string mMeshName;
	std::vector<glm::mat4> mInstanceMatrixList;

	bool isCreateinstancingBuffer = false;
public:
	void GenBuffers();
	void AddInstanceMatrix(const glm::mat4& matrix);
	bool HasBones() { return mHasBones; }
	std::vector<glm::mat4> GetInstanceMatrix() { return mInstanceMatrixList; }
	std::string GetName() { return mMeshName; }
	unsigned int GetMaterialIndex() { return mMaterialIndex; }
	Vertex * GetVerticesData(unsigned int &numVertex);
	unsigned int * GetIndicesData(unsigned int &numIndex);
	void Render();
	QHMesh(const aiMesh* mesh, const aiScene * scene, std::map<std::string, unsigned int> &BoneMapping, std::vector<BoneInfo> &BoneInfo);
	~QHMesh();
};