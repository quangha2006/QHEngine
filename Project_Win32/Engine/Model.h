#ifndef __MODEL_H__
#define __MODEL_H__

#include "Shader.h"
#include "Camera.h"
#include "TGA.h"
#include "PhysicsSimulation.h"
#include "QHMaterial.h"
#include "QHMesh.h"
#include "QHVertex.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <thread>

using namespace std;
enum RenderMode
{
	RenderMode_Material,
	RenderMode_Instancing,
	RenderMode_Auto
};

class Model
{
private:
	// Model data
	GLuint mVBO_material, mEBO_material, mVAO;
	vector<Texture> textures_loaded;
	vector<QHMaterial> mMaterial;
	vector<QHMesh> mQHMeshes;
	GLuint mNumVertices;
	GLuint mNumIndices;
	Vertex* mVertices_marterial;
	GLuint* mIndices_marterial;
	Shader mDept_Shader;
	Shader mModel_Shader;
	Shader mNormal_Shader;
	Shader mModel_WireFrame_Shader;
	bool mIsDrawWireFrame;
	void processNode(aiNode *node, const aiScene *scene, glm::mat4 nodeTransformation);

	void Pre_processNode(aiNode *node, const aiScene *scene, GLuint &numvertices, GLuint &numindices, GLuint &nummesh);

	void processMaterial(const aiScene* scene);
	void processMesh(const aiScene* scene, std::map<std::string, unsigned int> &BoneMapping, std::vector<BoneInfo> &BoneInfo);
	void BatchingVertexData();

	/*==== Animation ====*/
	Assimp::Importer mImporter;
	const aiScene* m_pScene;
	map<string, unsigned int> m_BoneMapping;
	vector<BoneInfo> m_BoneInfo;
	vector<glm::mat4> mTransforms;
	glm::mat4 m_GlobalInverseTransform;
	GLuint m_NumBones;
	bool m_hasAnimation;
	int animToPlay;
	int mNumAnimations;
	float mAnimationTime_begin, mAnimationTime_end;

	Camera *mCamera;
	bool m_initialized;
	bool mIsEnableAlpha;
	bool isDrawPolygon;
	bool uselighting;
	bool isUsePointLight;
	glm::vec3 customColor;
	bool useCustomColor;
	bool gammaCorrection;
	string mDirectory;
	string mSrcPath;
	bool mFlipUVs;
	float mFixedModel;
	int64_t mtimeStampAnim;
	bool mIsDrawDepthMap;
	bool mGammaCorrection;
	bool mIsVisible;
	glm::vec3 mScale;
	glm::vec3 mPos;
	glm::vec3 mRotate;
	float mAngle;
	glm::mat4 mWorldTransform;

	int m_Id;
	int m_meshdraw;
	RenderMode mRenderMode;
	bool mIsRenderNormalVisualization;
	float mNormalVisualizationMagnitude;
	btRigidBody* mRigidBody;
	glm::vec3 mFixedBoxShape;
	bool isDynamic;
	bool isFirstSetupUniform;
	bool mIsAutoRender;
	uint FindScaling(double AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(double AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindPosition(double AnimationTime, const aiNodeAnim* pNodeAnim);
	void ReadNodeHeirarchy(double AnimationTime, const aiNode* pNode, glm::mat4 &ParentTransform);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string &NodeName);
	void CalcInterpolatedScaling(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
	void UpdateWorldTransform();
	void SetWorld(glm::mat4 world = glm::mat4());
	bool CompileShader();
	void RenderNormalVisalization();
	void RenderBone();
	void GenBuffer();
public:
	void Init(const string &path, bool FlipUVs = true, bool enableAlpha = true, float fixedModel = 1.0f);
	void Render(RenderTargetType RT_Type);
	void SetRenderMode(RenderMode render_mode);
	void SetUseLighting(bool UseLighting);
	void SetisUsePointLight(bool UsePointLight);
	void DisableLightingForMesh(int numMesh);
	void SetCustomColor(glm::vec3 color);
	void SetTimeStampAnim(int64_t time);
	void SetRenderNormalVisualization(bool is_render, float magnitude = 0.2f);
	void UpdateAnimation();
	void SyncPhysics();
	void BoneTransform(int64_t TimeInSeconds, vector<glm::mat4> &Transforms);
	void SetScale(glm::vec3 scale);
	void SetScale(float scale);
	void SetPos(glm::vec3 pos);
	void SetRotate(float angle, glm::vec3 rotate);
	void SetAnimPlay(int anim);
	void SetDrawWireFrame(bool isdrawpolygon);
	void SetCamera(Camera * camera);
	void SetDrawMesh(int mesh);
	void SetVisible(bool isvisible);
	bool GetIsVisible();
	int GetAnimPlay();
	/**
	FULL Anim Time from 0.0 to 1.0
	**/
	void SetPlayAnimTime(float timeBegin, float timeEnd);
	std::string GetAnimNamePlaying();
	void Loading();
	void Translate(glm::vec3 trans);
	int GetId();
	glm::mat4 GetWorld();
	glm::vec3 GetScale();
	glm::vec3 GetPos();
	glm::vec3 GetRotate();
	void SetIsDrawDepthMap(bool isDraw);
	void CreateBoxShapePhysicsBody(float mass, glm::vec3 boxshape, glm::vec3 fixedboxshape = glm::vec3(0.));
	void CreateSphereShapePhysicsBody(float mass, float radius, glm::vec3 fixedboxshape = glm::vec3(0.));
	void ClearForcesPhysics();
	void CreateConvexHullShapePhysicsBody(float mass, bool isOptimize = true);
	void CreateConvexTriangleShapePhysicsBody(float mass, bool isOptimize = true);
	void CreateTriangleMeshShape(float mass);
	void CreateCapsuleShape(float mass, float radius, float height);

	btRigidBody* GetRigidBody();
	Model();
	~Model();
};
#endif // !__MODEL_H__