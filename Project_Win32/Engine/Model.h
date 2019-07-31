#ifndef __MODEL_H__
#define __MODEL_H__

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "TGA.h"
#include "ShaderManager.h"
#include "PhysicsSimulation.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
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

class Model
{
private:
	vector<Mesh*> mMeshes;
	vector<Texture> textures_loaded;
	void processNode(aiNode *node, const aiScene *scene, glm::mat4 nodeTransformation);
	Mesh *processMesh(aiMesh *mesh, const aiScene *scene, glm::mat4 nodeTransformation);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

	/*==== Animation ====*/
	Assimp::Importer importer, importer_anim;
	const aiScene* m_pScene, *m_pScene_anim;
	map<string, uint> m_BoneMapping;
	vector<BoneInfo> m_BoneInfo;
	vector<glm::mat4> Transforms;
	glm::mat4 m_GlobalInverseTransform;
	GLuint m_NumBones;
	bool hasAnimation;
	int animToPlay;
	int mNumAnimations;

	string useshadername;
	Camera *mCamera;
	bool m_initialized;
	bool mIsEnableAlpha;
	bool isDrawPolygon;
	bool uselighting;
	bool isUsePointLight;
	glm::vec3 customColor;
	bool useCustomColor;
	bool needRotate;
	bool gammaCorrection;
	string directory;
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

	btRigidBody* mRigidBody;
	glm::vec3 mFixedBoxShape;
	bool isDynamic;
	bool isFirstSetupUniform;
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

public:
	void Init(const string &path, bool FlipUVs = false, bool enableAlpha = true, float fixedModel = 1.0f);
	void Render(RenderMode mode, bool isTranslate = false, glm::vec3 translate = glm::vec3(), bool isRotate = false, float angle = 0.0f,glm::vec3 axis = glm::vec3(0.0f) );
	void SetUseLighting(bool UseLighting);
	void SetisUsePointLight(bool UsePointLight);
	void DisableLightingForMesh(int numMesh);
	void SetCustomColor(glm::vec3 color);
	void SetTimeStampAnim(int64_t time);
	void UpdateAnimation();
	void SyncPhysics();
	void BoneTransform(int64_t TimeInSeconds, vector<glm::mat4> &Transforms);
	void SetScale(glm::vec3 scale);
	void SetPos(glm::vec3 pos);
	void SetRotate(float angle, glm::vec3 rotate);
	void SetAnimPlay(int anim);
	void SetDrawPolygon(bool isdrawpolygon);
	void SetNeedRotate(bool isNeedRotate);
	void SetCamera(Camera * camera);
	void SetDrawMesh(int mesh);
	void SetId(int id);
	void SetVisible(bool isvisible);
	bool GetIsVisible();
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
	btRigidBody* GetRigidBody();
	Model();
	~Model();
};
#endif // !__MODEL_H__