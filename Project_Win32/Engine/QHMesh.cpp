#include "QHMesh.h"
#include "Logs.h"
#include "QHMath.h"
#include "Debugging.h"
#include "Utils.h"
#include "Timer.h"

void QHMesh::GenBuffers()
{
	if (mNumVertices <= 0 || mNumIndices <= 0)
		return;
	CheckGLError("QHMesh::GenBuffers before");
	int64_t time_begin = Timer::getMillisecond();
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);
	glGenBuffers(1, &mInstancingBuff);

	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mNumVertices * sizeof(Vertex), mVerticesData, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(GLuint), mIndicesData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weight));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, id));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindBuffer(GL_ARRAY_BUFFER, mInstancingBuff);
	glBufferData(GL_ARRAY_BUFFER, mInstanceMatrixList.size() * sizeof(glm::mat4), &mInstanceMatrixList[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(9);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(10);
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(11);
	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(8, 1);
	glVertexAttribDivisor(9, 1);
	glVertexAttribDivisor(10, 1);
	glVertexAttribDivisor(11, 1);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	int64_t time_end = Timer::getMillisecond();
	LOGI("GenBuffer: %u", time_end - time_begin);
	CheckGLError("QHMesh::GenBuffers");
}

void QHMesh::AddInstanceMatrix(const glm::mat4& matrix)
{
	mInstanceMatrixList.push_back(matrix);
}

Vertex * QHMesh::GetVerticesData(unsigned int & numVertex)
{
	numVertex = mNumVertices;
	return mVerticesData;
}

unsigned int * QHMesh::GetIndicesData(unsigned int & numIndex)
{
	numIndex = mNumIndices;
	return mIndicesData;
}

void QHMesh::Render()
{
	if (mNumIndices == 0U || mNumIndices == 0U)
		return;

	if (!isCreateinstancingBuffer)
	{
		GenBuffers();
		isCreateinstancingBuffer = true;
	}

	glBindVertexArray(mVAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

	if (mInstanceMatrixList.size() > 1)
	{
		QHEngine::DrawElementsInstanced(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0, mInstanceMatrixList.size());
	}
	else
	{
		QHEngine::DrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CheckGLError("QHMesh::Render");
}

QHMesh::QHMesh(const aiMesh* mesh, const aiScene * scene, std::map<std::string, unsigned int> &BoneMapping, std::vector<BoneInfo> &boneinfo)
	: mNumVertices(0)
	, mNumIndices(0)
	, mVerticesData(NULL)
	, mIndicesData(NULL)
	, mVBO(0)
	, mEBO(0)
	, mVAO(0)
	, mInstancingBuff(0)
{
	mHasBones = mesh->HasBones();
	mHasPos = mesh->HasPositions();
	mHasNormals = mesh->HasNormals();
	mHasTextureCoords0 = mesh->HasTextureCoords(0);
	mHasTangentsAndBitangents = mesh->HasTangentsAndBitangents();
	mHasVertexColors0 = mesh->HasVertexColors(0);
	mMaterialIndex = mesh->mMaterialIndex;
	mMeshName = mesh->mName.C_Str();
	// data to fill

	mNumVertices = mesh->mNumVertices;
	
	try
	{
		mVerticesData = new Vertex[mNumVertices];
	}
	catch (const std::bad_alloc&)
	{
		LOGE("Bad allocation : %ubyte\n", mNumVertices * sizeof(Vertex));
		mNumVertices = 0;
		return;
	}
	// Walk through each of the mesh's vertices
	if (!mHasNormals)
		LOGW("WARNING!!!: Mesh has no normal => disable lighting for this mesh\n");

	for (unsigned int i = 0; i < mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
						  // positions
		if (mHasPos)
		{
			const aiVector3D &pPos = mesh->mVertices[i];

			vector.x = pPos.x;
			vector.y = pPos.y;
			vector.z = pPos.z;

			vertex.Position = vector;
		}

		if (mHasNormals)
		{
			const aiVector3D &pNor = mesh->mNormals[i];
			vector.x = pNor.x;
			vector.y = pNor.y;
			vector.z = pNor.z;

			vertex.Normal = vector;
		}

		// texture coordinates
		if (mHasTextureCoords0) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;

		}

		if (mHasTangentsAndBitangents)
		{
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}

		if (mHasVertexColors0)
		{
			const aiColor4D &color = mesh->mColors[0][i];
			vertex.Color.r = color.r;
			vertex.Color.g = color.g;
			vertex.Color.b = color.b;
			vertex.Color.a = color.a;
		}
		else if (mMaterialIndex >=0 && mMaterialIndex < scene->mNumMaterials)
		{
			aiMaterial *material = scene->mMaterials[mMaterialIndex];
			aiColor3D kd_color(0.0f, 0.0f, 0.0f);
			float transparent = 1.0f;

			material->Get(AI_MATKEY_COLOR_DIFFUSE, kd_color);
			material->Get(AI_MATKEY_OPACITY, transparent);

			vertex.Color.r = kd_color.r;
			vertex.Color.g = kd_color.g;
			vertex.Color.b = kd_color.b;
			vertex.Color.a = transparent;
		}

		mVerticesData[i] = vertex;
	}

	// process Bones https://realitymultiplied.wordpress.com/2016/07/23/assimp-skeletal-animation-tutorial-2-loading-up-the-bone-data/

	if (mHasBones)
	{
		for (unsigned int i = 0; i < mesh->mNumBones; i++)
		{
			aiBone* aiBone = mesh->mBones[i]; //CREATING A POINTER TO THE CURRENT BONE
			unsigned int BoneIndex = i;
			std::string b_name = aiBone->mName.data;

			if (BoneMapping.find(b_name) == BoneMapping.end())
			{
				BoneIndex = BoneMapping.size();
				//m_NumBones++;
				BoneInfo bi;
				glm::mat4 b_mat = QHMath::AiToGLMMat4(aiBone->mOffsetMatrix);
				bi.BoneOffset = b_mat;
				boneinfo.push_back(bi);
				BoneMapping[b_name] = BoneIndex;
			}
			else
				BoneIndex = BoneMapping[b_name];

			for (unsigned int j = 0; j < aiBone->mNumWeights; j++)
			{
				aiVertexWeight weight = aiBone->mWeights[j];
				if (QHMath::compareFloat(mVerticesData[weight.mVertexId].weight.x, 0.0f))
				{
					mVerticesData[weight.mVertexId].id.x = (float)BoneIndex;
					mVerticesData[weight.mVertexId].weight.x = weight.mWeight;
					continue;
				}
				if (QHMath::compareFloat(mVerticesData[weight.mVertexId].weight.y, 0.0f))
				{
					mVerticesData[weight.mVertexId].id.y = (float)BoneIndex;
					mVerticesData[weight.mVertexId].weight.y = weight.mWeight;
					continue;
				}
				if (QHMath::compareFloat(mVerticesData[weight.mVertexId].weight.z, 0.0f))
				{
					mVerticesData[weight.mVertexId].id.z = (float)BoneIndex;
					mVerticesData[weight.mVertexId].weight.z = weight.mWeight;
					continue;
				}
				if (QHMath::compareFloat(mVerticesData[weight.mVertexId].weight.w, 0.0f))
				{
					mVerticesData[weight.mVertexId].id.w = (float)BoneIndex;
					mVerticesData[weight.mVertexId].weight.w = weight.mWeight;
					continue;
				}
				LOGW("Vertex at %u has more than 4 weight\n", weight.mVertexId);
			}
		}
	}

	// process Indices

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		mNumIndices += mesh->mFaces[i].mNumIndices;
	}

	try {
		mIndicesData = new unsigned int[mNumIndices];
	}
	catch (const std::bad_alloc&)
	{
		LOGE("Bad allocation : %ubyte\n", mNumIndices * sizeof(unsigned int));
		mNumIndices = 0;
		return;
	}

	unsigned int currentindex = 0;

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		std::memcpy(&mIndicesData[currentindex], face.mIndices, sizeof(unsigned int) * face.mNumIndices);
		currentindex+= face.mNumIndices;
	}

	
}


QHMesh::~QHMesh()
{
}
