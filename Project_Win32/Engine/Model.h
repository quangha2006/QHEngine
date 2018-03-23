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

unsigned int GenTextureId();
std::thread* TextureFromFile(const char *path, const string &directory, unsigned int textureID, bool gamma = false);
using namespace std;

class Model
{
private:
	vector<Mesh> meshes;
	vector<Texture> textures_loaded;
	void processNode(aiNode *node, const aiScene *scene, float fixedModel);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene, float fixedModel);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);

	/*==== Animation ====*/
	Assimp::Importer importer;
	const aiScene* m_pScene;
	map<string, uint> m_BoneMapping;
	vector<BoneInfo> m_BoneInfo;
	glm::mat4 m_GlobalInverseTransform;
	GLint m_NumBones;
	double mTicksPerSecond;
	double mDuration;
	bool hasAnimation;

	string useshadername;
	Camera *camera;
	bool isModelLoaded;
	bool isEnableAlpha;
	bool uselighting;
	glm::vec3 customColor;
	bool useCustomColor;
	bool needRotate;
	bool gammaCorrection;
	string directory;
public:
	void Init(string const &path, Camera *camera, bool enableAlpha, float fixedModel = 1.0f);
	void Draw(glm::mat4 model, glm::mat4 &lookat, glm::vec3 &lamppos);
	void SetUseLighting(bool UseLighting);
	void SetCustomColor(glm::vec3 color);
	void BoneTransform(float TimeInSeconds, vector<glm::mat4> &Transforms);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, glm::mat4 &ParentTransform);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
	}
	Model();
	~Model();
};