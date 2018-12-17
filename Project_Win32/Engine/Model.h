#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "TGA.h"
#include "ShaderManager.h"

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
	vector<Mesh> meshes;
	vector<Texture> textures_loaded;
	void processNode(aiNode *node, const aiScene *scene, float fixedModel);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene, float fixedModel);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType typeName);

	/*==== Animation ====*/
	Assimp::Importer importer, importer_anim;
	const aiScene* m_pScene, *m_pScene_anim;
	map<string, uint> m_BoneMapping;
	vector<BoneInfo> m_BoneInfo;
	vector<glm::mat4> Transforms;
	glm::mat4 m_GlobalInverseTransform;
	GLint m_NumBones;
	bool hasAnimation;
	int animToPlay;
	int mNumAnimations;

	string useshadername;
	Camera *camera;
	bool isModelLoaded;
	bool isEnableAlpha;
	bool isDrawPolygon;
	bool uselighting;
	bool isUsePointLight;
	glm::vec3 customColor;
	bool useCustomColor;
	bool needRotate;
	bool gammaCorrection;
	string directory;
	float timeStampAnim;

	glm::vec3 scale;
	glm::vec3 translate;
	glm::vec3 rotate;
	float angle;
	glm::mat4 world;

	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, glm::mat4 &ParentTransform);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void UpdateModel();

public:
	void Init(string const &path, Camera *camera, bool FlipUVs = false, bool enableAlpha = false, float fixedModel = 1.0f);
	void Draw(int drawmesh = -1, bool isTranslate = false, glm::vec3 translate = glm::vec3(), bool isRotate = false, float angle = 0.0f,glm::vec3 axis = glm::vec3(0.0f) );
	void SetUseLighting(bool UseLighting);
	void SetisUsePointLight(bool UsePointLight);
	void DisableLightingForMesh(int numMesh);
	void SetCustomColor(glm::vec3 color);
	void SetTimeStampAnim(int64_t time);
	void UpdateSkeleton(int64_t time = -1.0f);
	void BoneTransform(float TimeInSeconds, vector<glm::mat4> &Transforms);
	void SetScale(glm::vec3 scal);
	void SetTranslate(glm::vec3 trans);
	void SetRotate(float angle, glm::vec3 rotate);
	void SetWorld(glm::mat4 world = glm::mat4());
	void SetAnimPlay(int anim);
	void SetDrawPolygon(bool isdrawpolygon);
	void SetNeedRotate(bool isNeedRotate);
	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
	}
	Model();
	~Model();
};