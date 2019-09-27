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
	Vertex * mVerticesData;
	unsigned int * mIndicesData;
	unsigned int mMaterialIndex;
	GLuint mVBO, mEBO;

	bool mHasBones;
	bool mHasPos;
	bool mHasNormals;
	bool mHasTextureCoords0;
	bool mHasTangentsAndBitangents;
	bool mHasVertexColors0;

	void GenBuffers();
public:

	QHMesh(const aiMesh* mesh, std::map<std::string, unsigned int> &BoneMapping, std::vector<BoneInfo> &BoneInfo);
	~QHMesh();
};

