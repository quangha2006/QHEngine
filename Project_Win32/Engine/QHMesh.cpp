#include "QHMesh.h"
#include "Logs.h"
#include "QHMath.h"

void QHMesh::GenBuffers()
{
	if (mNumVertices <= 0 || mNumIndices <= 0)
		return;

	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mNumVertices * sizeof(Vertex), mVerticesData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(GLuint), mIndicesData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

QHMesh::QHMesh(const aiMesh* mesh, std::map<std::string, unsigned int> &BoneMapping, std::vector<BoneInfo> &boneinfo)
	: mNumVertices(0)
	, mNumIndices(0)
	, mVerticesData(NULL)
	, mIndicesData(NULL)
	, mVBO(0)
	, mEBO(0)
{
	mHasBones = mesh->HasBones();
	mHasPos = mesh->HasPositions();
	mHasNormals = mesh->HasNormals();
	mHasTextureCoords0 = mesh->HasTextureCoords(0);
	mHasTangentsAndBitangents = mesh->HasTangentsAndBitangents();
	mHasVertexColors0 = mesh->HasVertexColors(0);
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

	GenBuffers();

	mMaterialIndex = mesh->mMaterialIndex;
}


QHMesh::~QHMesh()
{
}
