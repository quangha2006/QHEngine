#include "stdafx.h"
#include "Model.h"
#include "Timer.h"
#include "Utils.h"
#include "QHTexture.h"
#include "ModelManager.h"
#include "Debugging.h"
#include "QHMath.h"
#include "RenderTarget.h"
#include "QHMaterial.h"
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

	unsigned int assimpFlag = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals;
	if (mFlipUVs)
		assimpFlag |= aiProcess_FlipUVs;;

	m_pScene = mImporter.ReadFile(path_modif, assimpFlag);

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

	processMaterial(m_pScene);

	uint64_t time_processMaterial = Timer::getMillisecond();
	LOGI("\nMaterial Count: %d\n", m_pScene->mNumMaterials);
	LOGI("ProcessMaterial time : %ums\n\n", (unsigned int)(time_processMaterial - time_loadmodel));

	processMesh(m_pScene, m_BoneMapping, m_BoneInfo);

	GLuint nummeshes = 0;

	Pre_processNode(m_pScene->mRootNode, m_pScene, mNumVertices, mNumIndices, nummeshes);

	LOGI("Mesh Count: %u/%u\n", nummeshes, m_pScene->mNumMeshes);
	LOGI("Vertices Count: %u totalMemory allocate: %u KB\n", mNumVertices, (sizeof(Vertex) * mNumVertices) / 1024);
	LOGI("Indices Count: %u\n", mNumIndices);

	processNode(m_pScene->mRootNode, m_pScene, glm::mat4());

	uint64_t time_processNode = Timer::getMillisecond();

	LOGI("ProcessNode time : %ums\n\n", (unsigned int)(time_processNode - time_processMaterial));

	LOGI("HasAnimations: %s\n", m_pScene->HasAnimations() ? "True" : "False");

	if (m_pScene->HasAnimations())
	{
		m_NumBones = m_BoneInfo.size();
		m_hasAnimation = true;
		mTransforms.resize(m_NumBones);
		mNumAnimations = m_pScene->mNumAnimations;
		LOGI("NumAnimation: %d\n", mNumAnimations);
	}

	BatchingVertexData();

	aiMemoryInfo meminfo;
	mImporter.GetMemoryRequirements(meminfo);
	LOGI("Memory Requirements: %uKB\n", meminfo.total / 1024);

	uint64_t time_ms_end = Timer::getMillisecond();

	

	if (!CompileShader())
		return;

	LOGI("Total Loading time : %ums\n", (unsigned int)(time_ms_end - time_ms_begin));
	m_initialized = true;
}
void Model::Pre_processNode(aiNode * node, const aiScene * scene, GLuint &numvertices, GLuint &numindices, GLuint &nummesh)
{
	nummesh += node->mNumMeshes;
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		numvertices += mesh->mNumVertices;

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			numindices += mesh->mFaces[i].mNumIndices;
		}

	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		Pre_processNode(node->mChildren[i], scene, numvertices, numindices, nummesh);
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

		QHMaterial material(aimaterial, mDirectory);

		mMaterial.push_back(material);
	}
}
void Model::processMesh(const aiScene * scene, std::map<std::string, unsigned int>& BoneMapping, std::vector<BoneInfo>& BoneInfo)
{
	if (!scene->HasMeshes())
		return;

	mQHMeshes.reserve(m_pScene->mNumMeshes);
	for (unsigned int i = 0; i < m_pScene->mNumMeshes; i++)
	{
		QHMesh SceneMesh(m_pScene->mMeshes[i], scene, m_BoneMapping, m_BoneInfo);
		mQHMeshes.push_back(SceneMesh);

	}
}
void Model::BatchingVertexData()
{
	uint64_t time_begin = Timer::getMillisecond();

	try
	{
		mVertices_marterial = new Vertex[mNumVertices];
		mIndices_marterial = new GLuint[mNumIndices];
	}
	catch (const std::bad_alloc&)
	{
		LOGE("Bad allocation : %ubyte\n", mNumVertices * sizeof(Vertex));
		mNumVertices = 0;
		mNumIndices = 0;
		return;
	}
	
	GLuint* indices_ptr = mIndices_marterial;
	Vertex* vertex_ptr = mVertices_marterial;
	GLuint last_vertex_index = 0;
	GLuint last_indices_index = 0;

	for (unsigned int i = 0; i < mMaterial.size(); i++)
	{
		mMaterial[i].mIndices_index = last_indices_index;
		//LOGI("Mesh: %d\n",i);
		for (QHMesh &currentMesh : mQHMeshes)
		{
			if (currentMesh.GetMaterialIndex() == i)
			{
				
				std::vector<glm::mat4> instanceMatrix = currentMesh.GetInstanceMatrix();
				
				if (mRenderMode == RenderMode_Auto && instanceMatrix.size() > 1)
					mRenderMode = RenderMode_Instancing;

				for (glm::mat4& localTransform : instanceMatrix)
				{
					unsigned int numVertex = 0;
					unsigned int numIndex = 0;
					Vertex* Vertex_ptr = currentMesh.GetVerticesData(numVertex);
					unsigned int* Indices_ptr = currentMesh.GetIndicesData(numIndex);

					if (numVertex == 0 || numIndex == 0 || !Vertex_ptr || !Indices_ptr)
						continue;

					unsigned int vertexCount = numVertex;
					do {
						Vertex tmp = *Vertex_ptr++;
						if (!currentMesh.HasBones())
						{
							glm::vec3 pos = localTransform * glm::vec4(tmp.Position, 1.0f);
							glm::vec3 normal = localTransform * glm::vec4(tmp.Normal, 0.0f);
							tmp.Position = pos;
							tmp.Normal = normal;
						}
						*vertex_ptr++ = tmp;
					} while (--vertexCount > 0);

					last_indices_index += numIndex;

					do {
						*indices_ptr++ = *Indices_ptr++ + last_vertex_index;
					} while (--numIndex > 0);

					last_vertex_index += numVertex;
				}
			}
		}
		mMaterial[i].mIndices_size = last_indices_index - mMaterial[i].mIndices_index;
	}

	//// sort transparent

	//unsigned int j = mMaterial.size() - 1;
	//unsigned int i = 0;
	//while (i < j)
	//{
	//	if (mMaterial[i].isTransparent())
	//	{
	//		std::swap(mMaterial[i], mMaterial[j]);
	//		// also swap material id in mesh
	//		for (unsigned int k = 0; k < mQHMeshes.size(); k++)
	//		{/*
	//			if (mMeshes[k].GetMaterialId() == i)
	//				mMeshes[k]SetMaterialId(j);
	//			if (mMeshes[k]->GetMaterialId() == j)
	//				mMeshes[k]->SetMaterialId(i);*/
	//		}
	//		--j;
	//	}
	//	else
	//	{
	//		++i;
	//	}
	//}
	if (mRenderMode == RenderMode_Auto)
		mRenderMode = RenderMode_Material;

	uint64_t time_end = Timer::getMillisecond();
	LOGI("SetupMaterialMesh time: %dms \n\n", ((int)(time_end - time_begin)));
}
void Model::processNode(aiNode * node, const aiScene * scene, glm::mat4 nodeTransformation)
{
	aiMatrix4x4 tmp = node->mTransformation;
	glm::mat4 currentNodeTransformation = QHMath::AiToGLMMat4(tmp);
	currentNodeTransformation = glm::transpose(currentNodeTransformation);
	glm::mat4 Transformation = QHMath::CombineMat4(currentNodeTransformation, nodeTransformation);
	//glm::mat4 Transformation = QHMath::CombineMat4(nodeTransformation, currentNodeTransformation);

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		unsigned int meshIndexNode = node->mMeshes[i];

		mQHMeshes[meshIndexNode].AddInstanceMatrix(Transformation);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, Transformation);
	}
}

void Model::Render(RenderTargetType RT_Type)
{
	if (!m_initialized || !mCamera || !mIsVisible) 
		return;

	// GenBuffer first
	if (mVBO_material == 0 && mEBO_material == 0 && mVAO == 0)
		GenBuffer();

	if (RT_Type == RenderTargetType_DEPTH && mIsDrawDepthMap == false) return;

	glm::mat4 WorldViewLightSpaceMatrix;
	glm::mat4 WorldViewProjectionMatrix;
	glm::mat4 model_inverse;
	glm::mat4 tmp_model = mWorldTransform;

	switch (RT_Type)
	{
	case RenderTargetType_DEPTH:

		mDept_Shader.use();

		mDept_Shader.setFloat("near_plane", mCamera->GetLightNear());
		mDept_Shader.setFloat("far_plane", mCamera->GetLightFar());
		
		WorldViewLightSpaceMatrix = mCamera->lightSpaceMatrix * tmp_model;
		mDept_Shader.setMat4("WorldViewLightSpaceMatrix", WorldViewLightSpaceMatrix);


		//animation
		if (m_hasAnimation && mTransforms.size() > 0)
		{
			mDept_Shader.setBoneMat4("gBones", mTransforms);

		}

		break;
	case RenderTargetType_COLOR:

		mModel_Shader.use();

		if (!isFirstSetupUniform) {
			mModel_Shader.setFloat("pointlight_constant", 1.0f);
			mModel_Shader.setFloat("pointlight_linear", 0.0014f);
			mModel_Shader.setFloat("pointlight_quadratic", 0.000007f);
			mModel_Shader.setFloat("material_shininess", 18.0f);
			mModel_Shader.setVec3("light_ambient", 0.7f, 0.7f, 0.7f);
			mModel_Shader.setVec3("light_diffuse", 1.0f, 1.0f, 1.0f); //light color
			mModel_Shader.setVec3("light_specular", 1.f, 1.f, 1.f);
			mModel_Shader.setBool("GammaCorrection", mGammaCorrection);
			isFirstSetupUniform = true;
		}

		mModel_Shader.setVec3("light_position", mCamera->GetLightPos());

		mModel_Shader.setVec3("viewPos", mCamera->GetPos());

		mModel_Shader.setBool("usepointlight", this->isUsePointLight);

		model_inverse = glm::inverse(tmp_model);
		model_inverse = glm::transpose(model_inverse);
		mModel_Shader.setMat4("world_inverse", model_inverse);

		mModel_Shader.setMat4("lightSpaceMatrix", mCamera->lightSpaceMatrix);
		mModel_Shader.setMat4("world", tmp_model);

		WorldViewProjectionMatrix = mCamera->GetWorldViewProjectionMatrix() * tmp_model;
		mModel_Shader.setMat4("WorldViewProjectionMatrix", WorldViewProjectionMatrix);


		//animation
		if (m_hasAnimation && mTransforms.size() > 0)
		{
			mModel_Shader.setBoneMat4("gBones", mTransforms);
		}
		
	}

	if (mRenderMode == RenderMode::RenderMode_Material || RT_Type == RenderTargetType::RenderTargetType_DEPTH)
	{
		glBindVertexArray(mVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO_material);
	}

	switch (RT_Type)
	{
	case RenderTargetType_DEPTH:

		QHEngine::DrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, (void*)0);

		break;
	case RenderTargetType_COLOR:
		if (mIsDrawWireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (mRenderMode == RenderMode::RenderMode_Material)
		{
			for (QHMaterial& material : mMaterial)
			{
				mModel_Shader.setBool("uselighting", uselighting);
				material.Apply(RT_Type, mModel_Shader, mIsDrawWireFrame, mIsEnableAlpha);
				material.Render();
			}
		}
		else if (mRenderMode == RenderMode::RenderMode_Instancing)
		{
			for (QHMesh& mesh : mQHMeshes)
			{
				mModel_Shader.setBool("uselighting", uselighting);
				unsigned int materialID = mesh.GetMaterialIndex();
				mMaterial[materialID].Apply(RT_Type, mModel_Shader, mIsDrawWireFrame, mIsEnableAlpha);
				mesh.Render();
			}
		}
		if (mIsDrawWireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		RenderNormalVisalization();

		RenderBone();
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::RenderNormalVisalization()
{
	if (!mIsRenderNormalVisualization)
		return;

	mNormal_Shader.use();
	mNormal_Shader.setMat4("projection", mCamera->GetProjection());
	mNormal_Shader.setMat4("view", mCamera->GetView());
	mNormal_Shader.setMat4("model", mWorldTransform);
	mNormal_Shader.setFloat("MAGNITUDE", mNormalVisualizationMagnitude);
	//animation
	if (m_hasAnimation && mTransforms.size() > 0)
	{
		mNormal_Shader.setBoneMat4("gBones", mTransforms);
	}
	if (mRenderMode == RenderMode::RenderMode_Material)
	{
		for (QHMaterial& material : mMaterial)
		{
			material.Render();
		}
	}
	else if (mRenderMode == RenderMode::RenderMode_Instancing)
	{
		for (QHMesh& mesh : mQHMeshes)
		{
			mesh.Render();
		}
	}
}

void Model::RenderBone()
{
	if (mTransforms.size() > 0)
	{
		Debugging::getInstance()->RenderBall(glm::vec3(0.0, 1.0, 0.0));
	}
}

void Model::GenBuffer()
{
	// create buffers/arrays
	if (mNumVertices > 0 && mNumIndices > 0)
	{
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO_material);
		glGenBuffers(1, &mEBO_material);

		glBindVertexArray(mVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO_material);
		glBufferData(GL_ARRAY_BUFFER, mNumVertices * sizeof(Vertex), mVertices_marterial, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO_material);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(GLuint), mIndices_marterial, GL_STATIC_DRAW);

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

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Model::SetRenderMode(RenderMode render_mode)
{
	if (!m_initialized)
		mRenderMode = render_mode;
	else
		LOGE("Cannot set RenderMode: you need to set before the model initialized\n");
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
	//if (numMesh >= 0 && numMesh < (int)mQHMeshes.size())
	//	mQHMeshes[numMesh].SetUseLighting(false);
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
void Model::SetRenderNormalVisualization(bool is_render, float magnitude)
{
	mIsRenderNormalVisualization = is_render;
	mNormalVisualizationMagnitude = magnitude;
}
void Model::UpdateAnimation()
{
	if (!m_initialized || !mCamera || !m_hasAnimation || !mIsVisible) return;

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
		btTransform trans = mRigidBody->getWorldTransform();

		btScalar matrix[16];
		trans.getOpenGLMatrix(matrix);

		glm::mat4 glm_mat4 = glm::make_mat4(matrix);

		glm_mat4 = glm::translate(glm_mat4, mFixedBoxShape);

		mWorldTransform = glm_mat4 * glm::scale(glm::mat4(), mScale);

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

void Model::SetScale(glm::vec3 scale)
{
	mScale = scale;
}
void Model::SetScale(float scale)
{
	mScale = glm::vec3(scale);
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
bool Model::CompileShader()
{
	std::string shader_define;

	if (m_hasAnimation)
	{
		shader_define += "#define SKINNED\n";
	}
	if (mRenderMode == RenderMode_Instancing)
	{
		shader_define += "#define INSTANCING\n";
	}

	if (!mDept_Shader.LoadShader("Shaders/DepthShader.vs", "Shaders/DepthShader.fs", false, shader_define.c_str()))
		return false;

	if (!mModel_Shader.LoadShader("Shaders/model_loading.vs", "Shaders/model_loading.fs", false, shader_define.c_str()))
		return false;

	mNormal_Shader.LoadShader("Shaders/normal_visualization.vs","Shaders/normal_visualization.fs","Shaders/normal_visualization.gs", false, shader_define.c_str());

	return true;
}
void Model::SetAnimPlay(int anim)
{
	if (anim >= 0 && (anim < mNumAnimations && mNumAnimations != -1))
		animToPlay = anim;
}
void Model::SetDrawWireFrame(bool isDrawWireFrame)
{
	mIsDrawWireFrame = isDrawWireFrame;
}

void Model::SetCamera(Camera * camera)
{
	mCamera = camera;
}

void Model::SetDrawMesh(int mesh)
{
	m_meshdraw = mesh;
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
	if (!m_pScene->HasAnimations() || animToPlay < 0 || animToPlay >= m_pScene->mNumAnimations) 
		return "";

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
	if (isDynamic)
		mRigidBody->setActivationState(DISABLE_DEACTIVATION);
}

void Model::CreateSphereShapePhysicsBody(float mass, float radius, glm::vec3 fixedboxshape)
{
	isDynamic = (mass != 0.f);
	mFixedBoxShape = fixedboxshape;
	mRigidBody = PhysicsSimulation::getInstance()->createSphereShape(mass, mPos + fixedboxshape, mRotate, mAngle, radius);
	if (isDynamic)
		mRigidBody->setActivationState(DISABLE_DEACTIVATION);
}

void Model::CreateConvexHullShapePhysicsBody(float mass, bool isOptimize)
{
	if (mNumVertices <= 0)
		return;

	isDynamic = (mass != 0.f);
	mRigidBody = PhysicsSimulation::getInstance()->createConvexHullShape(mass, mVertices_marterial, mNumVertices, mPos, mRotate, mAngle, mScale, isOptimize);
	if (isDynamic)
		mRigidBody->setActivationState(DISABLE_DEACTIVATION);
}

void Model::CreateConvexTriangleShapePhysicsBody(float mass, bool isOptimize)
{
	if (mNumVertices <= 0)
		return;

	isDynamic = (mass != 0.f);
	mRigidBody = PhysicsSimulation::getInstance()->createConvexTriangleMeshShape(mass, mVertices_marterial, mNumVertices, mIndices_marterial, mNumIndices, mPos, mRotate, mAngle, mScale);
	if (isDynamic)
		mRigidBody->setActivationState(DISABLE_DEACTIVATION);
}

void Model::CreateTriangleMeshShape(float mass)
{
	if (mNumVertices <= 0)
		return;

	isDynamic = (mass != 0.f);
	mRigidBody = PhysicsSimulation::getInstance()->createTriangleMeshShape(mass, mVertices_marterial, mNumVertices, mIndices_marterial, mNumIndices, mPos, mRotate, mAngle, mScale);
	if (isDynamic)
		mRigidBody->setActivationState(DISABLE_DEACTIVATION);
}

void Model::CreateCapsuleShape(float mass, float radius, float height)
{
	//if (mNumVertices <= 0) return;

	isDynamic = (mass != 0.f);
	mRigidBody = PhysicsSimulation::getInstance()->createCapsuleShape(mass, radius, height, mScale, mPos);
	if (isDynamic)
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

void Model::SetPlayAnimTime(float timeBegin, float timeEnd)
{
	QHMath::Clamp(timeBegin, 0.0f, 1.0f);
	QHMath::Clamp(timeEnd, 0.0f, 1.0f);

	mAnimationTime_begin = timeBegin;
	mAnimationTime_end = timeEnd;
}

void Model::BoneTransform(int64_t TimeInSeconds, vector<glm::mat4>& Transforms)
{
	glm::mat4 Identity = glm::mat4();
	if (animToPlay >= mNumAnimations)
		animToPlay = mNumAnimations - 1;
	double mTicksPerSecond = m_pScene->mAnimations[animToPlay]->mTicksPerSecond;
	double mDuration = m_pScene->mAnimations[animToPlay]->mDuration * mAnimationTime_end - m_pScene->mAnimations[animToPlay]->mDuration * mAnimationTime_begin;

	float TicksPerSecond = (float)(mTicksPerSecond != 0 ? mTicksPerSecond : 25.0f);
	double TimeInTicks = ((double)TimeInSeconds / 1000.0) * TicksPerSecond;
	double AnimationTime = fmod(TimeInTicks, mDuration) + m_pScene->mAnimations[animToPlay]->mDuration * mAnimationTime_begin;
	assert(AnimationTime >= 0.0);
	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

	for (uint i = 0; i < m_NumBones; i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}
}

void Model::ReadNodeHeirarchy(double AnimationTime, const aiNode * pNode, glm::mat4 & ParentTransform)
{
	string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[animToPlay];

	aiMatrix4x4 localTransForm = pNode->mTransformation;

	glm::mat4 NodeTransformation = QHMath::AiToGLMMat4(localTransForm);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling(1.0f);
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 ScalingM = glm::mat4();
		ScalingM = glm::scale(ScalingM, glm::vec3(Scaling.x, Scaling.y, Scaling.z));

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
		aiVector3D Translation(0.0f);
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		//InitTranslationTransform
		glm::mat4 TranslationM = glm::mat4();
		TranslationM[0][3] = Translation.x;
		TranslationM[1][3] = Translation.y;
		TranslationM[2][3] = Translation.z;

		// Combine the above transformations
		// TranslationM * RotationM * ScalingM
		glm::mat4 tmp1 = QHMath::CombineMat4(TranslationM, RotationM);
		NodeTransformation = QHMath::CombineMat4(tmp1, ScalingM);
	}

	glm::mat4 GlobalTransformation = QHMath::CombineMat4(ParentTransform, NodeTransformation);

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		uint BoneIndex = m_BoneMapping[NodeName];
		glm::mat4 tmp4 = QHMath::CombineMat4(GlobalTransformation, m_BoneInfo[BoneIndex].BoneOffset);
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
	if (pNodeAnim->mNumScalingKeys == 0)
	{
		return;
	}

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
	QHMath::Clamp(Factor, 0.0, 1.0);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + (float)Factor * Delta;
}

uint Model::FindScaling(double AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);
	uint i = 0;
	for (; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}
	return i-1;
}
void Model::CalcInterpolatedRotation(aiQuaternion & Out, double AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 0)
	{
		return;
	}

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
	QHMath::Clamp(Factor, 0.0, 1.0);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, (float)Factor); //ai_real = float
	Out = Out.Normalize();
}

uint Model::FindRotation(double AnimationTime, const aiNodeAnim * pNodeAnim)
{
	uint i = 0;
	for (; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}
	return i-1;
}

void Model::CalcInterpolatedPosition(aiVector3D & Out, double AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 0) {
		return;
	}

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
	QHMath::Clamp(Factor, 0.0, 1.0);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + (float)Factor * Delta;
}

uint Model::FindPosition(double AnimationTime, const aiNodeAnim * pNodeAnim)
{
	uint i = 0;
	for (; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}
	//assert(0);
	return i-1;
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
	, m_hasAnimation(false)
	, mNumAnimations(-1)
	, mtimeStampAnim(-1)
	, mAnimationTime_begin(0.0f)
	, mAnimationTime_end(1.0f)
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
	, mIsEnableAlpha(true)
	, mIsAutoRender(true)
	, mRigidBody(nullptr)
	, mVertices_marterial(nullptr)
	, mIndices_marterial(nullptr)
	, mNumVertices(0)
	, mNumIndices(0)
	, mRenderMode(RenderMode::RenderMode_Auto)
	, mIsDrawWireFrame(false)
	, mIsRenderNormalVisualization(false)
	, mNormalVisualizationMagnitude(0.2f)
	, mVBO_material(0)
	, mEBO_material(0)
	, mVAO(0)
{
	m_Id = ModelManager::getInstance()->AddModel(this);
}
Model::~Model()
{
	ModelManager::getInstance()->RemoveModel(m_Id);

	// Need implement this again
	for (unsigned int i = 0; i < textures_loaded.size(); i++)
		glDeleteTextures(1, &textures_loaded[i].id);

	glDeleteBuffers(1, &mVBO_material);
	glDeleteBuffers(1, &mEBO_material);

	if (mVertices_marterial != nullptr)
		delete[] mVertices_marterial;

	if (mIndices_marterial != nullptr)
		delete[] mIndices_marterial;
}