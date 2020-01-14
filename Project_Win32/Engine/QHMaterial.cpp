#include "QHMaterial.h"
#include "RenderManager.h"
#include "Debugging.h"
#include <map>

vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string &currentDirectory)
{
	static std::map<std::string, Texture> Textures_loaded;
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		if (str.length <= 0)
			continue;
		std::string texturePath = str.C_Str();
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		if (Textures_loaded.count(texturePath) > 0)
		{
			Texture texture = Textures_loaded[texturePath];
			textures.push_back(texture);
		}
		else  // if texture hasn't been loaded already, load it
		{
			Texture texture = QHTexture::TextureFromFile(texturePath, currentDirectory, type, 0, false);
			textures.push_back(texture);
			// store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			Textures_loaded[texturePath] = texture;
		}
	}

	return textures;
}

void QHMaterial::Apply(RenderTargetType RT_Type, Shader &modelShader, bool isDrawWireFrame, bool isEnableAlpha)
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;
	unsigned int texture_actived = 0;

	modelShader.setBool("useNormalMap", false);
	modelShader.setBool("enableAlpha", isEnableAlpha);

	for (; texture_actived < mTextures.size(); texture_actived++)
	{
		if (!isEnableAlpha && RT_Type == RenderTargetType_DEPTH) break;

		glActiveTexture(GL_TEXTURE0 + texture_actived); // active proper texture unit before binding
										  // retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name;
		aiTextureType texturetype = mTextures[texture_actived].type;
		if (texturetype == aiTextureType_DIFFUSE)
		{
			name = "texture_diffuse";
			ss << diffuseNr++; // transfer unsigned int to stream
		}
		else if (texturetype == aiTextureType_SPECULAR)
		{
			name = "texture_specular";
			ss << specularNr++; // transfer unsigned int to stream
		}
		else if (texturetype == aiTextureType_NORMALS || texturetype == aiTextureType_HEIGHT)
		{
			name = "texture_normal";
			ss << normalNr++; // transfer unsigned int to stream
			modelShader.setBool("useNormalMap", true);
		}
		number = ss.str();
		// now set the sampler to the correct texture unit
		string Material = "material_";
		string full_name = Material + name + number;

		modelShader.setInt(full_name.c_str(), texture_actived);

		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, mTextures[texture_actived].id);
	}
	if (RT_Type == RenderTargetType_COLOR)
	{
		GLuint depthmap = RenderManager::getInstance()->GetDepthMapId();
		if (depthmap > 0)
		{
			modelShader.setBool("useShadowMap", true);
			glActiveTexture(GL_TEXTURE0 + texture_actived);
			glBindTexture(GL_TEXTURE_2D, depthmap);
			modelShader.setInt("shadowMap", texture_actived);
		}
		else
			modelShader.setBool("useShadowMap", false);
	}

	modelShader.setFloat("material_transparent", mTransparent);
	modelShader.setFloat("material_shininess", mShininess);
	modelShader.setVec3("material_color_ambient", mAmbient);
	modelShader.setVec3("material_color_diffuse", mDiffuse);
	modelShader.setVec3("material_color_specular", mSpecular);
	modelShader.setBool("GammaCorrection", RenderManager::getInstance()->isEnableGammaCorrection());

	if (mShininess < 0.001f || !mHasNormals)
		modelShader.setBool("uselighting", false);

	if (!isEnableAlpha && RT_Type == RenderTargetType_DEPTH)
		modelShader.setBool("useTexture", false);

	if (mIsBackFace)
		glCullFace(GL_FRONT);
	else
		glCullFace(GL_BACK);

	if (isDrawWireFrame)
	{
		modelShader.setBool("useTexture", false);
	}
}

void QHMaterial::Render()
{
	if (mIndices_size <= 0) return;

	QHEngine::DrawElements(GL_TRIANGLES, mIndices_size, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * mIndices_index));
}

bool QHMaterial::isTransparent()
{
	if (mTransparent < 0.9f)
		return true;
	return false;
}

QHMaterial::QHMaterial(aiMaterial* aimaterial, std::string &currentDirectory)
	: mHasNormals(true)
	, mIndices_index(0)
	, mIndices_size(0)
{
	aiString name;
	aiColor3D ka_color(0.2f, 0.2f, 0.2f);
	aiColor3D kd_color(0.0f, 0.0f, 0.0f);
	aiColor3D ks_color(1.5f, 1.5f, 1.5f);
	float transparent = 1.0f;
	float shininess = 32.0f;
	float shininess_strength = 32.0f;
	float reflectivity = 1.0f;
	bool isbackface = false;

	aimaterial->Get(AI_MATKEY_NAME, name);

	aimaterial->Get(AI_MATKEY_COLOR_AMBIENT, ka_color);
	aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, kd_color);
	aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, ks_color);

	aimaterial->Get(AI_MATKEY_OPACITY, transparent);
	aimaterial->Get(AI_MATKEY_SHININESS, shininess);
	aimaterial->Get(AI_MATKEY_TWOSIDED, isbackface);
	aimaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength);
	aimaterial->Get(AI_MATKEY_REFLECTIVITY, reflectivity);

	// 1. diffuse maps
	std::vector<Texture> diffuseMaps = loadMaterialTextures(aimaterial, aiTextureType_DIFFUSE, currentDirectory);
	mTextures.insert(mTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture> specularMaps = loadMaterialTextures(aimaterial, aiTextureType_SPECULAR, currentDirectory);
	mTextures.insert(mTextures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(aimaterial, aiTextureType_NORMALS, currentDirectory);
	mTextures.insert(mTextures.end(), normalMaps.begin(), normalMaps.end());
	std::vector<Texture> heightMaps = loadMaterialTextures(aimaterial, aiTextureType_HEIGHT, currentDirectory);
	mTextures.insert(mTextures.end(), heightMaps.begin(), heightMaps.end());
	// 4. height maps
	std::vector<Texture> ambientMaps = loadMaterialTextures(aimaterial, aiTextureType_AMBIENT, currentDirectory);
	mTextures.insert(mTextures.end(), ambientMaps.begin(), ambientMaps.end());

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

	mAmbient = glm::vec3(ka_color.r, ka_color.g, ka_color.b);
	mDiffuse = glm::vec3(kd_color.r, kd_color.g, kd_color.b);
	mSpecular = glm::vec3(ks_color.r, ks_color.g, ks_color.b);
	mShininess = 300;
	mTransparent = transparent;
	mIsBackFace = isbackface;
	mMaterialName = name.C_Str();
}


QHMaterial::~QHMaterial()
{
}
