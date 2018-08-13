#include "stdafx.h"
#include "Model.h"
#include "Timer.h"
#include "Utils.h"
#include <SOIL.h>
#include <thread>
#include <Logs.h>
#ifdef _WINDOWS
#include <windows.h>
#endif
#include "AppContext.h"

glm::mat4 AiToGLMMat4(aiMatrix4x4& in_mat)
{
	glm::mat4 tmp;
	tmp[0][0] = in_mat.a1;	tmp[0][1] = in_mat.a2;	tmp[0][2] = in_mat.a3;	tmp[0][3] = in_mat.a4;
	tmp[1][0] = in_mat.b1;	tmp[1][1] = in_mat.b2;	tmp[1][2] = in_mat.b3;	tmp[1][3] = in_mat.b4;
	tmp[2][0] = in_mat.c1;	tmp[2][1] = in_mat.c2;	tmp[2][2] = in_mat.c3;	tmp[2][3] = in_mat.c4;
	tmp[3][0] = in_mat.d1;	tmp[3][1] = in_mat.d2;	tmp[3][2] = in_mat.d3;	tmp[3][3] = in_mat.d4;

	return tmp;
}

glm::mat4 testfun(glm::mat4 a, glm::mat4 b)
{
	glm::mat4 result(0.0f);
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			result[i][j] = a[i][0] * b[0][j] +
				a[i][1] * b[1][j] +
				a[i][2] * b[2][j] +
				a[i][3] * b[3][j];
		}
	}
	return result;
}
Model::Model()
{
	gammaCorrection = false;
	isModelLoaded = false;
	useshadername = "model";
	uselighting = true;
	useCustomColor = false;
	customColor = glm::vec3(1.0f);
	m_NumBones = 0;
	m_pScene = NULL;
	hasAnimation = false;
	timeStampAnim = -1;
	scale = glm::vec3(1.0f);
	translate = glm::vec3(0.0f);
	rotate = glm::vec3(0.0f);
	angle = 0.0f;
	this->world = glm::mat4();
	animToPlay = 0;
	isDrawPolygon = false;
}
Model::~Model()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].DeleteBuffer();

	for (unsigned int i = 0; i < textures_loaded.size(); i++)
		glDeleteTextures(1, &textures_loaded[i].id);
}
void Model::Init(string const & path, Camera *camera, bool enableAlpha, float fixedModel)
{
	uint64_t time_ms_begin = Timer::getMillisecond();
	this->camera = camera;
	this->isEnableAlpha = enableAlpha;
	string path_modif = Utils::getResourcesFolder() + path;

	LOGI("\nLoad Model: %s\n", path_modif.c_str());
	if (ShaderManager::getInstance()->GetProgram(useshadername.c_str()) == 0)
	{
		LOGE("ERROR! modelShader is invalid!");
		return;
	}

	// read file via ASSIMP
	//Assimp::Importer importer;
	m_pScene = importer.ReadFile(path_modif, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
	if ((path_modif.find_last_of(".dae") == (path_modif.length() - 1)) || (path_modif.find_last_of(".md5mesh") == (path_modif.length() - 1)))
		needRotate = true;
	else
		needRotate = false;
	uint64_t time_loadmodel = Timer::getMillisecond();
	LOGI("Load Model time : %.3fs\n", ((int)(time_loadmodel - time_ms_begin)) / 1000.0f);
	// check for errors
	if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path_modif.substr(0, path_modif.find_last_of('/'));
	LOGI("Mesh Count: %d\n", m_pScene->mNumMeshes);
	LOGI("Material Count: %d\n", m_pScene->mNumMaterials);
	LOGI("HasAnimations: %s\n", m_pScene->HasAnimations()?"True":"False");
	if (m_pScene->HasAnimations())
	{
		hasAnimation = true;
		m_GlobalInverseTransform = glm::inverse(AiToGLMMat4(m_pScene->mRootNode->mTransformation));
		mNumAnimations = m_pScene->mNumAnimations;
		LOGI("NumAnimation: %d\n", mNumAnimations);
	}
	// process ASSIMP's root node recursively
	processNode(m_pScene->mRootNode, m_pScene, fixedModel);

	isModelLoaded = true;

	uint64_t time_ms_end = Timer::getMillisecond();

	LOGI("Total Loading time : %.3fs\n", ((int)(time_ms_end - time_ms_begin)) / 1000.0f);

}

void Model::processNode(aiNode * node, const aiScene * scene, float fixedModel)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh meshIndex = processMesh(mesh, scene, fixedModel);
		meshes.push_back(meshIndex);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, fixedModel);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene, float fixedModel)
{
	int WEIGHTS_PER_VERTEX = 4;
	int boneArraysSize = mesh->mNumVertices * WEIGHTS_PER_VERTEX;
	bool hasnormals = false;
	bool hasbone = false;
	// data to fill
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	vector<float> boneWeights(boneArraysSize, 0.0f);
	
	
	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
						  // positions
		const aiVector3D* pPos = &(mesh->mVertices[i]);
		vector.x = pPos->x * fixedModel;
		vector.y = pPos->y * fixedModel;
		vector.z = pPos->z * fixedModel;
		
		vertex.Position = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			hasnormals = true;
		}
		else
			vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
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
		//if (mesh->HasVertexColors(i))
		//{
		//	//aiColor4D test = mesh->;
		//}
		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process Bones https://realitymultiplied.wordpress.com/2016/07/23/assimp-skeletal-animation-tutorial-2-loading-up-the-bone-data/

	if (scene->HasAnimations())
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
				glm::mat4 b_mat = AiToGLMMat4(aiBone->mOffsetMatrix);
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
				for (unsigned int k = 0; k < WEIGHTS_PER_VERTEX; k++)
				{
					if (boneWeights.at(vertexStart + k) == 0.0f)
					{
						boneWeights.at(vertexStart + k) = weight.mWeight;
						//boneIDs.at(vertexStart + k) = BoneIndex;
						switch(k)
						{
						case 0:
							vertices.at(weight.mVertexId).id.x = (float)BoneIndex;
							vertices.at(weight.mVertexId).weight.x = weight.mWeight;
							break;
						case 1:
							vertices.at(weight.mVertexId).id.y = (float)BoneIndex;
							vertices.at(weight.mVertexId).weight.y = weight.mWeight;
							break;
						case 2:
							vertices.at(weight.mVertexId).id.z = (float)BoneIndex;
							vertices.at(weight.mVertexId).weight.z = weight.mWeight;
							break;
						case 3:
							vertices.at(weight.mVertexId).id.w = (float)BoneIndex;
							vertices.at(weight.mVertexId).weight.w = weight.mWeight;
							break;
						}
						break;
					}
				}
			}

		}
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// Get material color
	aiColor3D ka_color(0.0f, 0.0f, 0.0f);
	aiReturn result_get_color = material->Get(AI_MATKEY_COLOR_AMBIENT, ka_color);
	if (result_get_color == aiReturn_FAILURE)
	{
		//LOGI("Get AMBIENT failed!\n");
	}
	aiColor3D kd_color(0.0f, 0.0f, 0.0f);
	result_get_color = material->Get(AI_MATKEY_COLOR_DIFFUSE, kd_color);
	if (result_get_color == aiReturn_FAILURE)
	{
		//LOGI("Get DIFFUSE failed!\n");
	}

	aiColor3D ks_color(0.0f, 0.0f, 0.0f);
	result_get_color = material->Get(AI_MATKEY_COLOR_SPECULAR, ks_color);
	
	if (result_get_color == aiReturn_FAILURE)
	{
		//LOGI("Get SPECULAR failed!\n");
	}

	float transparent = 1.0f;
	material->Get(AI_MATKEY_OPACITY, transparent);

	float shininess = 1.0f;
	material->Get(AI_MATKEY_SHININESS, shininess);

	Material mesh_material;
	mesh_material.ambient = glm::vec3(ka_color.r, ka_color.g, ka_color.b);
	mesh_material.diffuse = glm::vec3(kd_color.r, kd_color.g, kd_color.b);
	mesh_material.specular = glm::vec3(ks_color.r, ks_color.g, ks_color.b);
	mesh_material.shininess = shininess;
	mesh_material.transparent = transparent;
	
	return Mesh(vertices, indices, textures, mesh_material, string(mesh->mName.C_Str()), hasnormals, hasbone);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = GenTextureId();
			TextureFromFile(str.C_Str(), this->directory, texture.id);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void Model::Draw(glm::vec3 &lamppos)
{
	if (!isModelLoaded || !camera) return;

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	//ShaderManager::getInstance()->setUseProgram(useshadername.c_str());
	glm::mat4 tmp_model = world;

	if (needRotate)
	{
		tmp_model = glm::rotate(tmp_model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		tmp_model = glm::rotate(tmp_model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	ShaderManager::getInstance()->setMat4("world", tmp_model);
	glm::mat4 lookat_tmp = camera->WorldViewProjectionMatrix * tmp_model;
	
	ShaderManager::getInstance()->setMat4("WorldViewProjectionMatrix", lookat_tmp);

	glm::mat4 model_inverse = glm::inverse(tmp_model);
	model_inverse = glm::transpose(model_inverse);
	ShaderManager::getInstance()->setMat4("world_inverse", model_inverse);

	ShaderManager::getInstance()->setFloat("material_shininess", 18.0f);
	ShaderManager::getInstance()->setVec3("light_position", lamppos);
	ShaderManager::getInstance()->setVec3("viewPos", camera->Pos);
	ShaderManager::getInstance()->setVec3("light_ambient", 0.7f, 0.7f, 0.7f);
	ShaderManager::getInstance()->setVec3("light_diffuse", 1.0f, 1.0f, 1.0f); //light color
	ShaderManager::getInstance()->setVec3("light_specular", 1.1f, 1.1f, 1.1f);
	ShaderManager::getInstance()->setFloat("pointlight_constant", 1.0f);
	ShaderManager::getInstance()->setFloat("pointlight_linear", 0.007f);
	ShaderManager::getInstance()->setFloat("pointlight_quadratic", 0.0002f);
	ShaderManager::getInstance()->setBool("enableAlpha", this->isEnableAlpha);
	ShaderManager::getInstance()->setBool("usenormalmap", false);
	ShaderManager::getInstance()->setVec3("color_pick", 0.0f, 0.0f, 0.0f);

	//animation
	if (hasAnimation && Transforms.size() > 0)
	{
		int m_boneLocation = glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), "gBones");
		if (m_boneLocation >= 0)
			glUniformMatrix4fv(m_boneLocation, Transforms.size(), GL_TRUE, glm::value_ptr(Transforms[0]));
	}

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		ShaderManager::getInstance()->setBool("uselighting", uselighting);
		meshes[i].Draw(useCustomColor, customColor);
	}
		
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void Model::SetUseLighting(bool UseLighting)
{
	this->uselighting = UseLighting;
}
void Model::DisableLightingForMesh(int numMesh)
{
	if (numMesh >= 0 && numMesh < this->meshes.size())
		meshes[numMesh].SetUseLighting(false);
}
void Model::SetCustomColor(glm::vec3 color)
{
	useCustomColor = true;
	customColor = color;
}
void Model::SetTimeStampAnim(int64_t time)
{
	timeStampAnim = (float)(time / 1000.0f);
}
void Model::UpdateTransform(int64_t time)
{
	if (!hasAnimation) return;

	Transforms.clear();

	float RunningTime = 0.0f;

	if (timeStampAnim < 0.0f)
		RunningTime = (float)(Timer::getMillisecond() / 1000.0f);
	else
		RunningTime = timeStampAnim;

	if (time != -1)
		RunningTime = time;
	BoneTransform(RunningTime, Transforms);
}
void Model::BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms)
{
	glm::mat4 Identity = glm::mat4();

	double mTicksPerSecond = m_pScene->mAnimations[animToPlay]->mTicksPerSecond;
	double mDuration = m_pScene->mAnimations[animToPlay]->mDuration;

	float TicksPerSecond = (float)(mTicksPerSecond != 0 ? mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)mDuration);

	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);
	Transforms.resize(m_NumBones);

	for (uint i = 0; i < m_NumBones; i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}
}
void Model::SetScale(glm::vec3 scal)
{
	this->scale = scal;

	this->world = glm::scale(this->world, scal);
}
void Model::SetTranslate(glm::vec3 trans)
{
	this->translate = trans;

	this->world = glm::translate(this->world, trans);
}
void Model::SetRotate(float angle, glm::vec3 rotate)
{
	this->angle = angle;

	this->rotate = rotate;

	this->world = glm::rotate(this->world, glm::radians(angle), rotate);
}
void Model::SetWorld(glm::mat4 model)
{
	this->world = model;
}
void Model::SetAnimPlay(int anim)
{
	if (anim >= 0 && anim < mNumAnimations)
		animToPlay = anim;
}
void Model::SetDrawPolygon(bool isdrawpolygon)
{
	if (isdrawpolygon == isDrawPolygon)
		return;

	isDrawPolygon = isdrawpolygon;

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].SetDrawPolygon(isDrawPolygon);
	}
}
void Model::ReadNodeHeirarchy(float AnimationTime, const aiNode * pNode, glm::mat4 & ParentTransform)
{
	string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[animToPlay];

	aiMatrix4x4 tmp = pNode->mTransformation;

	glm::mat4 NodeTransformation = AiToGLMMat4(tmp);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

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
		glm::mat4 tmp1 = testfun(TranslationM, RotationM);
		glm::mat4 tmp2 = testfun(tmp1, ScalingM);
		NodeTransformation = tmp2;
	}

	glm::mat4 GlobalTransformation = testfun(ParentTransform, NodeTransformation);

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		uint BoneIndex = m_BoneMapping[NodeName];
		glm::mat4 tmp3 = testfun(m_GlobalInverseTransform, GlobalTransformation);
		glm::mat4 tmp4 = testfun(tmp3, m_BoneInfo[BoneIndex].BoneOffset);
		m_BoneInfo[BoneIndex].FinalTransformation = tmp4;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}
const aiNodeAnim * Model::FindNodeAnim(const aiAnimation * pAnimation, const string NodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}
void Model::CalcInterpolatedScaling(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}
void Model::UpdateModel()
{
	this->world = glm::mat4();

	this->world = glm::scale(this->world, this->scale);

	if (this->angle > 0.0f || this->angle < 0.0f)
		this->world = glm::rotate(this->world, glm::radians(this->angle), this->rotate);

	this->world = glm::translate(this->world, this->translate);
}
uint Model::FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
void Model::CalcInterpolatedRotation(aiQuaternion & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}
void Model::CalcInterpolatedPosition(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}
uint Model::FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
uint Model::FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
void SOIL_loader(string fullpath, unsigned int textureID/*, GLFWwindow* win*/)
{
	//bool MakeCurrent = eglMakeCurrent(ESContext::getInstance()->eglDisplay, ESContext::getInstance()->eglSurface, ESContext::getInstance()->eglSurface, esContex_loader);
	CheckGLError("MakeCurrent");
	//glfwMakeContextCurrent(win);
	//if (!MakeCurrent)
	{
		//LOGI("eglMakeCurrent() returned error %d\n", eglGetError());
	}
	
	if (strlen(fullpath.c_str()) > 3)
	{
		string filetype = fullpath.substr(fullpath.length() - 3, 3);
		if (!filetype.compare("tga") || !filetype.compare("TGA"))
		{
			uint64_t time_begin = Timer::getMillisecond();
			TGA::TextureData texture;
			if (LoadTGA(texture, fullpath.c_str()))
			{
				//EnterCriticalSection(&CriticalSection);
				glBindTexture(GL_TEXTURE_2D, textureID);
				// Set the filtering mode
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexImage2D(GL_TEXTURE_2D, 0, texture.type, texture.width, texture.height, 0, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
				glGenerateMipmap(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				CheckGLError("glTexImage2D");
				//LeaveCriticalSection(&CriticalSection);
				uint64_t time_end = Timer::getMillisecond();
				LOGI("ENDLOAD TGA  Load: %dms %s\n", (int)(time_end - time_begin), fullpath.c_str());
			}
			else
				LOGE("ERROR when load: %s \n", fullpath.c_str());
		}
		else
		{
			uint64_t time_begin = Timer::getMillisecond();
			if (SOIL_load_OGL_texture(fullpath.c_str(), SOIL_LOAD_RGBA, textureID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS) == 0)
			{
				LOGE("ERROR when load: %s \n", fullpath.c_str());
				return;
			}
			uint64_t time_end = Timer::getMillisecond();
			LOGI("ENDLOAD SOIL Load: %dms %s\n", (int)(time_end - time_begin), fullpath.c_str());
		}
	}
	//glfwDestroyWindow(win);
	//eglMakeCurrent(ESContext::getInstance()->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}
unsigned int GenTextureId()
{
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);
	CheckGLError("GenTextureId");
	return textureID;
}
std::thread* TextureFromFile(const char * path, const string & directory, unsigned int textureID, bool gamma)
{
	string fullpath = directory + '/' + string(path);
	//LOGI("Load texture: %s \n", fullpath.c_str());

	//EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE, EGL_NONE };
	//EGLConfig config;
	//EGLint nNumConfigs;
	//eglGetConfigs(ESContext::getInstance()->eglDisplay, &config, 1, &nNumConfigs);
	//EGLContext esContex_loader;// = eglCreateContext(ESContext::getInstance()->eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
	
	/*CheckGLError("esContex_loader");
	GLFWwindow* win;
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	int a = AppContext::getInstance()->GetWindowWidth();
	int b = AppContext::getInstance()->GetWindowHeight();

	win = glfwCreateWindow(AppContext::getInstance()->GetWindowWidth(), AppContext::getInstance()->GetWindowHeight(), "Optimus example", 0, AppContext::getInstance()->GetWindow());
*/
	//std::thread *loading = new thread(SOIL_loader, fullpath,textureID, win);
	SOIL_loader(fullpath, textureID); // no thread
	//loading->join();
	return nullptr;
}