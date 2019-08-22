#include "stdafx.h"
#include "Model.h"
#include "Timer.h"
#include "Utils.h"
#include "QHTexture.h"
#include "ModelManager.h"
#include "Debugging.h"
#include "QHMath.h"
#include "RenderTarget.h"
#include <SOIL.h>
#include <thread>
#include <Logs.h>
#ifdef _WINDOWS
#include <windows.h>
#endif
#include "AppContext.h"

void Model::Init(const string &path, bool FlipUVs, bool enableAlpha, float fixedModel)
{
	if (m_initialized)
	{
		LOGE("initialized: %s", path.c_str());
	}
	mSrcPath = path;
	mIsEnableAlpha = enableAlpha;
	mFixedModel = fixedModel;
	mFlipUVs = FlipUVs;
}

void Model::Loading() //thread
{
	if (mSrcPath == "") return;

	uint64_t time_ms_begin = Timer::getMillisecond();
	string path_modif = Utils::getResourcesFolder() + mSrcPath;

	LOGW("\n======================================================\n");

	LOGI("\nLoad Model: %s\n", path_modif.c_str());

	unsigned int assimpFlag = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices;
	if (mFlipUVs)
		assimpFlag |= aiProcess_FlipUVs;;

	m_pScene = mImporter.ReadFile(path_modif, assimpFlag);

	if ((path_modif.find_last_of(".dae") == (path_modif.length() - 1)) || (path_modif.find_last_of(".md5mesh") == (path_modif.length() - 1)))
		needRotate = true;
	else
		needRotate = false;

	uint64_t time_loadmodel = Timer::getMillisecond();
	LOGI("Load Model time : %ums\n\n", (unsigned int)(time_loadmodel - time_ms_begin));
	// check for errors
	if (!m_pScene || /*m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||*/ !m_pScene->mRootNode) // if is Not Zero
	{
		LOGE("ERROR::ASSIMP:: %s\n", mImporter.GetErrorString());
		return;
	}
	// retrieve the directory path of the filepath
	mDirectory = path_modif.substr(0, path_modif.find_last_of('/'));
	// process ASSIMP's root node recursively
	aiMatrix4x4 tmp = m_pScene->mRootNode->mTransformation;
	glm::mat4 NodeTransformation = QHMath::AiToGLMMat4(tmp);
	NodeTransformation = glm::transpose(NodeTransformation);

	GLuint numvertices = 0;
	GLuint numindices = 0;
	GLuint nummeshes = 0;
	Pre_processNode(m_pScene->mRootNode, m_pScene, numvertices, numindices, nummeshes);

	mVertices = new Vertex[numvertices];
	mIndices = new GLuint[numindices];
	mMeshes.reserve(nummeshes);

	processNode(m_pScene->mRootNode, m_pScene, NodeTransformation);

	uint64_t time_processNode = Timer::getMillisecond();
	LOGI("Mesh Count: %d\n", m_pScene->mNumMeshes);
	LOGI("Vertices Count: %d\n", mNumVertices);
	LOGI("Indices Count: %d\n", mNumIndices);
	LOGI("ProcessNode time : %ums\n\n", (unsigned int)(time_processNode - time_loadmodel));

	processMaterial(m_pScene);

	uint64_t time_processMaterial = Timer::getMillisecond();
	LOGI("\nMaterial Count: %d\n", m_pScene->mNumMaterials);
	LOGI("ProcessMaterial time : %ums\n\n", (unsigned int)(time_processMaterial - time_processNode));

	
	LOGI("HasAnimations: %s\n", m_pScene->HasAnimations() ? "True" : "False");

	m_GlobalInverseTransform = glm::inverse(QHMath::AiToGLMMat4(m_pScene->mRootNode->mTransformation));

	if (m_pScene->HasAnimations())
	{
		hasAnimation = true;
		mTransforms.resize(m_NumBones);
		mNumAnimations = m_pScene->mNumAnimations;
		LOGI("NumAnimation: %d\n", mNumAnimations);
	}

	SetupMaterialMesh();

	// create buffers/arrays
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);
	glGenBuffers(1, &mVBO_material);
	glGenBuffers(1, &mEBO_material);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mNumVertices * sizeof(Vertex), mVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(GLuint), mIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO_material);
	glBufferData(GL_ARRAY_BUFFER, mNumVertices * sizeof(Vertex), mVertices_marterial, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO_material);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(GLuint), mIndices_marterial, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uint64_t time_ms_end = Timer::getMillisecond();

	LOGI("Total Loading time : %ums\n", (unsigned int)(time_ms_end - time_ms_begin));

	m_initialized = true;
}
void Model::Pre_processNode(aiNode * node, const aiScene * scene, GLuint &numvertices, GLuint &numindices, GLuint &nummesh)
{
	nummesh += node->mNumMeshes;
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Pre_processMesh(mesh, scene, numvertices, numindices);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		Pre_processNode(node->mChildren[i], scene, numvertices, numindices, nummesh);
	}
}
void Model::Pre_processMesh(aiMesh * mesh, const aiScene * scene, GLuint &numvertices, GLuint &numindices)
{
	numvertices += mesh->mNumVertices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		numindices += mesh->mFaces[i].mNumIndices;
	}
}
void Model::processMaterial(const aiScene * scene)
{
	if (!scene->HasMaterials())
		return;

	unsigned int nummaterial = scene->mNumMaterials;

	mMaterial.reserve(nummaterial);

	for (unsigned int i = 0; i < nummaterial; i++)
	{
		aiMaterial* aimaterial = scene->mMaterials[i];

		aiColor3D ka_color(0.0f, 0.0f, 0.0f);
		aiColor3D kd_color(0.0f, 0.0f, 0.0f);
		aiColor3D ks_color(0.5f, 0.5f, 0.5f);
		float transparent = 1.0f;
		float shininess = 1.0f;

		aimaterial->Get(AI_MATKEY_COLOR_AMBIENT, ka_color);
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, kd_color);
		aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, ks_color);
		aimaterial->Get(AI_MATKEY_OPACITY, transparent);
		aimaterial->Get(AI_MATKEY_SHININESS, shininess);

		QHMaterial material;
		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(aimaterial, aiTextureType_DIFFUSE);
		material.mTextures.insert(material.mTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(aimaterial, aiTextureType_SPECULAR);
		material.mTextures.insert(material.mTextures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(aimaterial, aiTextureType_NORMALS);
		material.mTextures.insert(material.mTextures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> heightMaps = loadMaterialTextures(aimaterial, aiTextureType_HEIGHT);
		material.mTextures.insert(material.mTextures.end(), heightMaps.begin(), heightMaps.end());
		// 4. height maps
		std::vector<Texture> ambientMaps = loadMaterialTextures(aimaterial, aiTextureType_AMBIENT);
		material.mTextures.insert(material.mTextures.end(), ambientMaps.begin(), ambientMaps.end());

		if (diffuseMaps.size() > 1)
			LOGW("Num MaterialTexture DIFFUSE = %d\n", diffuseMaps.size());

		if (specularMaps.size() > 1)
			LOGW("Num MaterialTexture SPECULAR = %d\n", specularMaps.size());

		if (normalMaps.size() > 1)
			LOGW("Num MaterialTexture NORMALS = %d\n", normalMaps.size());

		if (heightMaps.size() > 1)
			LOGW("Num MaterialTexture HEIGHT = %d\n", heightMaps.size());

		if (ambientMaps.size() > 1)
			LOGW("Num MaterialTexture AMBIENT = %d\n", ambientMaps.size());

		if (diffuseMaps.size() > 0)
			kd_color.r = -1;

		if (specularMaps.size() > 0)
			ks_color.r = -1;

		material.mAmbient = glm::vec3(ka_color.r, ka_color.g, ka_color.b);
		material.mDiffuse = glm::vec3(kd_color.r, kd_color.g, kd_color.b);
		material.mSpecular = glm::vec3(ks_color.r, ks_color.g, ks_color.b);
		material.mShininess = shininess;
		material.mTransparent = transparent;
		material.mHasNormals = true;
		mMaterial.push_back(material);
	}
}
void Model::SetupMaterialMesh()
{
	uint64_t time_begin = Timer::getMillisecond();

	mVertices_marterial = new Vertex[mNumVertices];
	mIndices_marterial = new GLuint[mNumIndices];
	GLuint* indices_ptr = mIndices_marterial;
	GLuint last_vertex_index = 0;
	GLuint last_indices_index = 0;

	for (GLuint i = 0; i < mMaterial.size(); i++)
	{
		mMaterial[i].mIndices_index = last_indices_index;

		for (GLuint j = 0; j < mMeshes.size(); j++)
		{
			if (mMeshes[j]->GetMaterialId() == i)
			{
				GLuint numvertex = 0;
				GLuint numindices = 0;
				Vertex* vertex = mMeshes[j]->GetVertex(numvertex);
				GLuint* indices_mesh_ptr = mMeshes[j]->GetIndices(numindices);
				if (numindices == 0 || numvertex == 0) continue;
				std::memcpy(&mVertices_marterial[last_vertex_index], vertex, sizeof(Vertex) * numvertex);

				//for (GLuint k = 0; k < numindices; k++)
				//{
					//mIndices_marterial[k + last_indices_index] = indices[k] + last_vertex_index;
				//}
				
				last_indices_index += numindices;

				do {
					*indices_ptr++ = *indices_mesh_ptr++ + last_vertex_index;
				} while (--numindices > 0);

				last_vertex_index += numvertex;
				
			}
		}
		mMaterial[i].mIndices_size = last_indices_index - mMaterial[i].mIndices_index;
	}

	uint64_t time_end = Timer::getMillisecond(); 
	LOGI("SetupMaterialMesh time: %dms \n\n", ((int)(time_end - time_begin)));
}
void Model::processNode(aiNode * node, const aiScene * scene, glm::mat4 nodeTransformation)
{
	// process each mesh located at the current node
	aiMatrix4x4 tmp = node->mTransformation;
	glm::mat4 currentNodeTransformation = QHMath::AiToGLMMat4(tmp);
	currentNodeTransformation = glm::transpose(currentNodeTransformation);
	glm::mat4 Transformation = QHMath::Combinetransformations(nodeTransformation, currentNodeTransformation);

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh *meshIndex = processMesh(mesh, scene, Transformation);
		mMeshes.push_back(meshIndex);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, Transformation);
	}
}

Mesh *Model::processMesh(aiMesh * mesh, const aiScene * scene, glm::mat4 nodeTransformation)
{
	unsigned char WEIGHTS_PER_VERTEX = 4;
	int boneArraysSize = mesh->mNumVertices * WEIGHTS_PER_VERTEX;
	bool hasnormals = false;
	bool hasbone = false;
	bool hasanim = scene->HasAnimations();
	// data to fill
	vector<Texture> textures;
	vector<float> boneWeights(boneArraysSize, 0.0f);

	unsigned int numvertices_prev_mesh = mNumVertices;

	unsigned int indices_index = mNumIndices;

	unsigned int numvertices = mesh->mNumVertices;

	// Walk through each of the mesh's vertices
	if (!mesh->HasNormals())
		LOGW("WARNING!!!: Mesh has no normal => disable lighting for this mesh\n");

	for (unsigned int i = 0; i < numvertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
						  // positions
		const aiVector3D* pPos = &(mesh->mVertices[i]);
		vector.x = pPos->x;
		vector.y = pPos->y;
		vector.z = pPos->z;
		if (hasanim)
			vertex.Position = vector;
		else
			vertex.Position = nodeTransformation * glm::vec4(vector, 1.0f);
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			if (hasanim)
				vertex.Normal = vector;
			else
				vertex.Normal = nodeTransformation * glm::vec4(vector, 1.0f);
			hasnormals = true;
		}
		else
		{
			vertex.Normal = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;

		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		if (mesh->HasTangentsAndBitangents())
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
		else
		{
			vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
			vertex.Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		mVertices[mNumVertices++] = vertex;
	}

	// process Bones https://realitymultiplied.wordpress.com/2016/07/23/assimp-skeletal-animation-tutorial-2-loading-up-the-bone-data/
	
	if (hasanim)
	{
		if (mesh->mNumBones > 0)
			hasbone = true;
		for (unsigned int i = 0; i < mesh->mNumBones; i++)
		{
			aiBone* aiBone = mesh->mBones[i]; //CREATING A POINTER TO THE CURRENT BONE
			GLint BoneIndex = i;
			std::string b_name = aiBone->mName.data;
			
			if (m_BoneMapping.find(b_name) == m_BoneMapping.end())
			{
				BoneIndex = m_NumBones;
				m_NumBones++;
				BoneInfo bi;
				glm::mat4 b_mat = QHMath::AiToGLMMat4(aiBone->mOffsetMatrix);
				bi.BoneOffset = b_mat;
				m_BoneInfo.push_back(bi);
				m_BoneMapping[b_name] = BoneIndex;
			}
			else
				BoneIndex = m_BoneMapping[b_name];

			for (unsigned int j = 0; j < aiBone->mNumWeights; j++)
			{
				aiVertexWeight weight = aiBone->mWeights[j];
				unsigned int vertexStart = weight.mVertexId * WEIGHTS_PER_VERTEX;
				for (unsigned char k = 0; k < WEIGHTS_PER_VERTEX; k++)
				{
					if (boneWeights.at(vertexStart + k) == 0.0f)
					{
						boneWeights.at(vertexStart + k) = weight.mWeight;
						switch(k)
						{
						case 0:
							mVertices[weight.mVertexId + numvertices_prev_mesh].id.x = (float)BoneIndex;
							mVertices[weight.mVertexId + numvertices_prev_mesh].weight.x = weight.mWeight;
							break;
						case 1:
							mVertices[weight.mVertexId + numvertices_prev_mesh].id.y = (float)BoneIndex;
							mVertices[weight.mVertexId + numvertices_prev_mesh].weight.y = weight.mWeight;
							break;
						case 2:
							mVertices[weight.mVertexId + numvertices_prev_mesh].id.z = (float)BoneIndex;
							mVertices[weight.mVertexId + numvertices_prev_mesh].weight.z = weight.mWeight;
							break;
						case 3:
							mVertices[weight.mVertexId + numvertices_prev_mesh].id.w = (float)BoneIndex;
							mVertices[weight.mVertexId + numvertices_prev_mesh].weight.w = weight.mWeight;
							break;
						}
						break;
					}
				}
			}

		}
	}
	Vertex *verCurrentMesh = new Vertex[numvertices];
	//clone vertex => import to each mesh
	std::memcpy(verCurrentMesh, &mVertices[numvertices_prev_mesh], sizeof(Vertex) * numvertices);
	// process Indices
	unsigned int numIndices = 0; // for this mesh

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		numIndices += mesh->mFaces[i].mNumIndices;
	}
	GLuint *indicesCurrentMesh = new GLuint[numIndices];
	GLuint currentindex = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			mIndices[mNumIndices++] = face.mIndices[j] + numvertices_prev_mesh;
			indicesCurrentMesh[currentindex++] = face.mIndices[j];
		}
	}

	Mesh *mesh_current = new Mesh(indices_index, numIndices, mesh->mMaterialIndex, string(mesh->mName.C_Str()), hasnormals, hasbone);
	
	mesh_current->SetVertex(verCurrentMesh, numvertices);
	mesh_current->SetIndices(indicesCurrentMesh, numIndices);

	return mesh_current;
}

vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		if (str.length <= 0) continue;
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture = QHTexture::TextureFromFile(str.C_Str(), mDirectory, type, 0, false);
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}

	return textures;
}

void Model::Render(RenderTargetType RT_Type, bool isTranslate, glm::vec3 translate, bool isRotate, float angle, glm::vec3 axis)
{
	if (!m_initialized || !mCamera || !mIsVisible || mVBO == 0 || mEBO == 0) return;
	if (RT_Type == RenderTargetType_DEPTH && mIsDrawDepthMap == false) return;

	bool render_model_mode = true;

	glm::mat4 WorldViewLightSpaceMatrix;
	glm::mat4 WorldViewProjectionMatrix;
	glm::mat4 model_inverse;
	glm::mat4 tmp_model = mWorldTransform;

	if (isTranslate)
		tmp_model = glm::translate(tmp_model, translate);

	if (isRotate)
		tmp_model = glm::rotate(tmp_model, glm::radians(angle), axis);

	if (needRotate)
	{
		tmp_model = glm::rotate(tmp_model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		tmp_model = glm::rotate(tmp_model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	switch (RT_Type)
	{
		case RenderTargetType_DEPTH:
			if (hasAnimation)
				ShaderManager::getInstance()->setUseProgram("depthShader_skinning");
			else
				ShaderManager::getInstance()->setUseProgram("depthShader");

			ShaderSet::setFloat("near_plane", mCamera->light_near);
			ShaderSet::setFloat("far_plane", mCamera->light_far);

			WorldViewLightSpaceMatrix = mCamera->lightSpaceMatrix * tmp_model;
			ShaderSet::setMat4("WorldViewLightSpaceMatrix", WorldViewLightSpaceMatrix);

			break;
		case RenderTargetType_COLOR:
			if (hasAnimation)
				ShaderManager::getInstance()->setUseProgram("model_skinning");
			else
				ShaderManager::getInstance()->setUseProgram("model");
			if (!isFirstSetupUniform) {
				ShaderSet::setFloat("pointlight_constant", 1.0f);
				ShaderSet::setFloat("pointlight_linear", 0.0014f);
				ShaderSet::setFloat("pointlight_quadratic", 0.000007f);
				ShaderSet::setFloat("material_shininess", 18.0f);
				ShaderSet::setVec3("light_ambient", 0.7f, 0.7f, 0.7f);
				ShaderSet::setVec3("light_diffuse", 1.0f, 1.0f, 1.0f); //light color
				ShaderSet::setVec3("light_specular", 1.f, 1.f, 1.f);
				ShaderSet::setBool("GammaCorrection", mGammaCorrection);
				isFirstSetupUniform = true;
			}

			ShaderSet::setVec3("light_position", mCamera->lightPos);

			ShaderSet::setVec3("viewPos", mCamera->Pos);

			ShaderSet::setBool("usepointlight", this->isUsePointLight);
			ShaderSet::setBool("enableAlpha", mIsEnableAlpha); //temp

			model_inverse = glm::inverse(tmp_model);
			model_inverse = glm::transpose(model_inverse);
			ShaderSet::setMat4("world_inverse", model_inverse);

			ShaderSet::setMat4("lightSpaceMatrix", mCamera->lightSpaceMatrix);
			ShaderSet::setMat4("world", tmp_model);

			WorldViewProjectionMatrix = mCamera->WorldViewProjectionMatrix * tmp_model;
			ShaderSet::setMat4("WorldViewProjectionMatrix", WorldViewProjectionMatrix);
			break;
	}
	
	if (render_model_mode)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVBO_material);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO_material);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	}


	Shader* modelShader = ShaderManager::getInstance()->GetCurrentShader();

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

	//animation
	if (hasAnimation && mTransforms.size() > 0)
	{
		ShaderSet::setBoneMat4("gBones", mTransforms);
	}

	switch (RT_Type)
	{
	case RenderTargetType_DEPTH:
		QHEngine::DrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, (void*)0);
		break;
	case RenderTargetType_COLOR:
		if (render_model_mode)
		{
			for (GLuint i = 0; i < mMaterial.size(); i++)
			{
				ShaderSet::setBool("uselighting", uselighting);
				mMaterial[i].Apply(RT_Type, mIsEnableAlpha);
				mMaterial[i].Draw();
			}
		}
		else
		{
			if (m_meshdraw > -1)
			{
				if (m_meshdraw < (int)mMeshes.size())
				{
					ShaderSet::setBool("uselighting", uselighting);
					mMaterial[mMeshes[m_meshdraw]->GetMaterialId()].Apply(RT_Type, mIsEnableAlpha);
					mMeshes[m_meshdraw]->Draw(RT_Type, useCustomColor, customColor);
				}
			}
			else
			{
				for (unsigned int i = 0; i < mMeshes.size(); i++)
				{
					ShaderSet::setBool("uselighting", uselighting);
					mMaterial[mMeshes[i]->GetMaterialId()].Apply(RT_Type, mIsEnableAlpha);
					mMeshes[i]->Draw(RT_Type, useCustomColor, customColor);
				}
			}
		}
		break;
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::SetUseLighting(bool UseLighting)
{
	this->uselighting = UseLighting;
}
void Model::SetisUsePointLight(bool UsePointLight)
{
	this->isUsePointLight = UsePointLight;
}
void Model::DisableLightingForMesh(int numMesh)
{
	if (numMesh >= 0 && numMesh < (int)mMeshes.size())
		mMeshes[numMesh]->SetUseLighting(false);
}
void Model::SetCustomColor(glm::vec3 color)
{
	useCustomColor = true;
	customColor = color;
}
void Model::SetTimeStampAnim(int64_t time)
{
	mtimeStampAnim = time;
}
void Model::UpdateAnimation()
{
	if (!m_initialized || !mCamera || !hasAnimation || !mIsVisible) return;

	int64_t RunningTime = 0;

	if (mtimeStampAnim < 0)
		RunningTime = Timer::getMillisecond();
	else
		RunningTime = mtimeStampAnim;

	BoneTransform(RunningTime, mTransforms);
}
void Model::SyncPhysics()
{
	if (m_initialized && isDynamic && mRigidBody != nullptr)
	{
		btTransform trans = mRigidBody->getWorldTransform();;

		//mRigidBody->getWorldTransform(trans);
		//mRigidBody->set
		btQuaternion rotation = trans.getRotation();
		float x = float(trans.getOrigin().getX());
		float y = float(trans.getOrigin().getY());
		float z = float(trans.getOrigin().getZ());

		btScalar angle = rotation.getAngle();
		btScalar ro_x = rotation.getAxis().getX();
		btScalar ro_y = rotation.getAxis().getY();
		btScalar ro_z = rotation.getAxis().getZ();

		
		mPos = glm::vec3(x, y, z);

		mWorldTransform = glm::mat4();

		mWorldTransform = glm::scale(mWorldTransform, mScale);

		mWorldTransform = glm::translate(mWorldTransform, mPos / mScale);

		mWorldTransform = rotate(mWorldTransform, rotation.getAngle(), vec3(ro_x, ro_y, ro_z));

		mWorldTransform = glm::translate(mWorldTransform, - mFixedBoxShape / mScale);

	}
	else if (m_initialized)
	{
		UpdateWorldTransform();
	}
}

void Model::UpdateWorldTransform()
{
	if (!m_initialized) return;

	glm::mat4 rota = rotate(glm::mat4(), glm::radians(mAngle), mRotate);

	mWorldTransform = glm::mat4();

	mWorldTransform = glm::scale(mWorldTransform, mScale);

	mWorldTransform = glm::translate(mWorldTransform, (mPos / mScale));

	if (mAngle > 0.0f || mAngle < 0.0f)
		mWorldTransform *= rota;

	

	mWorldTransform = glm::translate(mWorldTransform, -mFixedBoxShape);
}

void Model::Translate(glm::vec3 trans)
{
	mPos += trans;

	if (mRigidBody != nullptr)
	{
		btTransform transform = mRigidBody->getWorldTransform();
		btVector3 tr(mPos.x, mPos.y, mPos.z);
		transform.setOrigin(tr);
		mRigidBody->setWorldTransform(transform);
	}
}

void Model::BoneTransform(int64_t TimeInSeconds, vector<glm::mat4>& Transforms)
{
	glm::mat4 Identity = glm::mat4();
	if (animToPlay >= mNumAnimations) 
		animToPlay = mNumAnimations -1;
	double mTicksPerSecond = m_pScene->mAnimations[animToPlay]->mTicksPerSecond;
	double mDuration = m_pScene->mAnimations[animToPlay]->mDuration;

	float TicksPerSecond = (float)(mTicksPerSecond != 0 ? mTicksPerSecond : 25.0f);
	double TimeInTicks = ((double)TimeInSeconds / 1000.0) * TicksPerSecond;
	double AnimationTime = fmod(TimeInTicks, mDuration);

	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

	for (uint i = 0; i < m_NumBones; i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}
}
void Model::SetScale(glm::vec3 scale)
{
	mScale = scale;
}
void Model::SetPos(glm::vec3 pos)
{
	mPos = pos;
	mWorldTransform = glm::translate(mWorldTransform, (mPos / mScale));
}
void Model::SetRotate(float angle, glm::vec3 rotate)
{
	mAngle += angle;

	mRotate = rotate;

	if (mRigidBody != nullptr)
	{
		btTransform transform = mRigidBody->getWorldTransform();
		btQuaternion quater(btVector3(rotate.x, rotate.y, rotate.z), glm::radians(mAngle));
		transform.setRotation(quater);
		mRigidBody->setWorldTransform(transform);
	}
}
void Model::SetWorld(glm::mat4 world)
{
	mWorldTransform = world;
}
void Model::SetAnimPlay(int anim)
{
	if (anim >= 0 && (anim < mNumAnimations && mNumAnimations != -1))
		animToPlay = anim;
}
void Model::SetDrawPolygon(bool isdrawpolygon)
{
	if (isdrawpolygon == isDrawPolygon)
		return;

	isDrawPolygon = isdrawpolygon;

	for (unsigned int i = 0; i < mMeshes.size(); i++)
	{
		mMeshes[i]->SetDrawPolygon(isDrawPolygon);
	}
}
void Model::SetNeedRotate(bool isNeedRotate)
{
	this->needRotate = isNeedRotate;
}

void Model::SetCamera(Camera * camera)
{
	mCamera = camera;
}

void Model::SetDrawMesh(int mesh)
{
	m_meshdraw = mesh;
}

void Model::SetId(int id)
{
	m_Id = id;
}

void Model::SetVisible(bool isvisible)
{
	mIsVisible = isvisible;
}

bool Model::GetIsVisible()
{
	return mIsVisible;
}

int Model::GetAnimPlay()
{
	return animToPlay;
}

std::string Model::GetAnimNamePlaying()
{
	if (!m_pScene->HasAnimations() || animToPlay < 0 || animToPlay >= m_pScene->mNumAnimations) return "";

	aiString animName = m_pScene->mAnimations[animToPlay]->mName;
	
	return std::string(animName.C_Str());
}

int Model::GetId()
{
	return m_Id;
}

void Model::SetIsDrawDepthMap(bool isDraw)
{
	mIsDrawDepthMap = isDraw;
}

void Model::CreateBoxShapePhysicsBody(float mass, glm::vec3 boxshape, glm::vec3 fixedboxshape)
{
	isDynamic = (mass != 0.f);
	mFixedBoxShape = fixedboxshape;
	mRigidBody = PhysicsSimulation::getInstance()->createBoxShape(mass, mPos + fixedboxshape, mRotate, mAngle, boxshape);
	mRigidBody->setActivationState(DISABLE_DEACTIVATION);
}

void Model::CreateSphereShapePhysicsBody(float mass, float radius, glm::vec3 fixedboxshape)
{
	isDynamic = (mass != 0.f);
	mFixedBoxShape = fixedboxshape;
	mRigidBody = PhysicsSimulation::getInstance()->createSphereShape(mass, mPos + fixedboxshape, mRotate, mAngle, radius);
	mRigidBody->setActivationState(DISABLE_DEACTIVATION);
}

void Model::ClearForcesPhysics()
{
	if (mRigidBody != nullptr)
	{
		btVector3 zeroVector(0, 0, 0);

		mRigidBody->clearForces();
		mRigidBody->setLinearVelocity(zeroVector);
		mRigidBody->setAngularVelocity(zeroVector);
	}
}

btRigidBody * Model::GetRigidBody()
{
	return mRigidBody;
}

void Model::ReadNodeHeirarchy(double AnimationTime, const aiNode * pNode, glm::mat4 & ParentTransform)
{
	string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[animToPlay];

	aiMatrix4x4 tmp = pNode->mTransformation;

	glm::mat4 NodeTransformation = QHMath::AiToGLMMat4(tmp);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	glm::mat4 GlobalTransformation = QHMath::Combinetransformations(ParentTransform, NodeTransformation);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 ScalingM = glm::mat4();
		ScalingM = glm::scale(ScalingM, glm::vec3(Scaling.x, Scaling.y, Scaling.z));
		//InitScaleTransform(ScalingM, Scaling.x, Scaling.y, Scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		aiMatrix3x3 tmpmat3 = RotationQ.GetMatrix();
		glm::mat4 RotationM = glm::mat4(glm::make_mat3(&tmpmat3.a1));
		RotationM[0][3] = 0.0f; 
		RotationM[1][3] = 0.0f; 
		RotationM[2][3] = 0.0f; 
		RotationM[3][0] = 0.0f; RotationM[3][1] = 0.0f; RotationM[3][2] = 0.0f; RotationM[3][3] = 1.0f;
		
		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);

		//InitTranslationTransform
		glm::mat4 TranslationM = glm::mat4();
		TranslationM[0][3] = Translation.x;
		TranslationM[1][3] = Translation.y;
		TranslationM[2][3] = Translation.z;

		// Combine the above transformations
		glm::mat4 tmp1 = QHMath::Combinetransformations(TranslationM, RotationM);
		glm::mat4 tmp2 = QHMath::Combinetransformations(tmp1, ScalingM);
		NodeTransformation = tmp2;
		GlobalTransformation = QHMath::Combinetransformations(ParentTransform, NodeTransformation);
	}

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		uint BoneIndex = m_BoneMapping[NodeName];
		glm::mat4 tmp3 = QHMath::Combinetransformations(m_GlobalInverseTransform, GlobalTransformation);
		glm::mat4 tmp4 = QHMath::Combinetransformations(tmp3, m_BoneInfo[BoneIndex].BoneOffset);
		m_BoneInfo[BoneIndex].FinalTransformation = tmp4;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}
const aiNodeAnim * Model::FindNodeAnim(const aiAnimation * pAnimation, const string &NodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}
void Model::CalcInterpolatedScaling(aiVector3D & Out, double AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	double DeltaTime = pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime;
	double Factor = (AnimationTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + (float)Factor * Delta;
}

uint Model::FindScaling(double AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}
void Model::CalcInterpolatedRotation(aiQuaternion & Out, double AnimationTime, const aiNodeAnim * pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	double DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
	double Factor = (AnimationTime - pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, (float)Factor); //ai_real = float
	Out = Out.Normalize();
}
void Model::CalcInterpolatedPosition(aiVector3D & Out, double AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	double DeltaTime = pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime;
	double Factor = (AnimationTime - pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + (float)Factor * Delta;
}
uint Model::FindRotation(double AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}
uint Model::FindPosition(double AnimationTime, const aiNodeAnim * pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}
glm::mat4 Model::GetWorld()
{
	return mWorldTransform;
}
glm::vec3 Model::GetScale()
{
	return mScale;
}
glm::vec3 Model::GetPos()
{
	return mPos;
}
glm::vec3 Model::GetRotate()
{
	return mRotate;
}
Model::Model()
	: gammaCorrection(false)
	, m_initialized(false)
	, uselighting(true)
	, useCustomColor(false)
	, customColor(glm::vec3(1.0f))
	, m_NumBones(0)
	, m_pScene(NULL)
	, hasAnimation(false)
	, mNumAnimations(-1)
	, mtimeStampAnim(-1)
	, mScale(glm::vec3(1.0f))
	, mPos(glm::vec3(0.0f))
	, mRotate(glm::vec3(0.0f))
	, mAngle(0.0f)
	, mWorldTransform(glm::mat4())
	, animToPlay(0)
	, isDrawPolygon(false)
	, isUsePointLight(false)
	, m_Id(-1)
	, mSrcPath("")
	, mIsDrawDepthMap(true)
	, mCamera(Camera::getInstance())
	, m_meshdraw(-1)
	, mGammaCorrection(false)
	, isDynamic(false)
	, mIsVisible(true)
	, mFixedBoxShape(glm::vec3(0.))
	, isFirstSetupUniform(false)
	, mRigidBody(nullptr)
	, mVertices(nullptr)
	, mIndices(nullptr)
	, mVertices_marterial(nullptr)
	, mIndices_marterial(nullptr)
	, mNumVertices(0)
	, mNumIndices(0)
{
	ModelManager::getInstance()->AddModel(this);
}
Model::~Model()
{
	ModelManager::getInstance()->RemoveModel(m_Id);

	for (unsigned int i = 0; i < mMeshes.size(); i++)
	{
		delete mMeshes[i];
	}
	
	for (unsigned int i = 0; i < textures_loaded.size(); i++)
		glDeleteTextures(1, &textures_loaded[i].id);

	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);
	if (mVertices != nullptr)
		delete[] mVertices;

	if (mIndices != nullptr)
		delete[] mIndices;

	if (mVertices_marterial != nullptr)
		delete[] mVertices_marterial;

	if (mIndices_marterial != nullptr)
		delete[] mIndices_marterial;
}