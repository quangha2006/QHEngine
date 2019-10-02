#include "QHMaterial.h"
#include "ShaderManager.h"
#include "RenderManager.h"
#include "Debugging.h"

vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string &currentDirectory)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		if (str.length <= 0) continue;
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		//for (unsigned int j = 0; j < textures_loaded.size(); j++)
		//{
		//	if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0)
		//	{
		//		textures.push_back(textures_loaded[j]);
		//		skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
		//		break;
		//	}
		//}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture = QHTexture::TextureFromFile(str.C_Str(), currentDirectory, type, 0, false);
			textures.push_back(texture);
			//textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}

	return textures;
}

void QHMaterial::Apply(RenderTargetType RT_Type, bool isDrawWireFrame, bool isEnableAlpha)
{

	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;
	unsigned int texture_actived = 0;

	ShaderSet::setBool("useNormalMap", false);
	ShaderSet::setBool("enableAlpha", isEnableAlpha);

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
			ShaderSet::setBool("useNormalMap", true);
		}
		number = ss.str();
		// now set the sampler to the correct texture unit
		string Material = "material_";
		string full_name = Material + name + number;
		//int temp = glGetUniformLocation(ShaderSet::GetCurrentProgram(), full_name.c_str());

		ShaderSet::setInt(full_name.c_str(), texture_actived);

		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, mTextures[texture_actived].id);
	}
	if (RT_Type == RenderTargetType_COLOR)
	{
		GLuint depthmap = RenderManager::getInstance()->GetDepthMapId();
		if (depthmap > 0)
		{
			ShaderSet::setBool("useShadowMap", true);
			glActiveTexture(GL_TEXTURE0 + texture_actived);
			glBindTexture(GL_TEXTURE_2D, depthmap);
			ShaderSet::setInt("shadowMap", texture_actived);
		}
		else
			ShaderSet::setBool("useShadowMap", false);
	}

	ShaderSet::setFloat("material_transparent", mTransparent);
	ShaderSet::setFloat("material_shininess", mShininess);
	ShaderSet::setVec3("material_color_ambient", mAmbient);
	ShaderSet::setVec3("material_color_diffuse", mDiffuse);
	ShaderSet::setVec3("material_color_specular", mSpecular);
	ShaderSet::setBool("GammaCorrection", RenderManager::getInstance()->isEnablemGammaCorrection());
	if (mShininess < 0.001f || !mHasNormals)
		ShaderSet::setBool("uselighting", false);

	if (!isEnableAlpha && RT_Type == RenderTargetType_DEPTH)
		ShaderSet::setBool("useTexture", false);
	if (mIsBackFace)
		glCullFace(GL_FRONT);
	else
		glCullFace(GL_BACK);

	if (isDrawWireFrame)
	{
		ShaderSet::setBool("useTexture", false);
		return;
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
	aiColor3D ka_color(0.0f, 0.0f, 0.0f);
	aiColor3D kd_color(0.0f, 0.0f, 0.0f);
	aiColor3D ks_color(0.5f, 0.5f, 0.5f);
	float transparent = 1.0f;
	float shininess = 1.0f;
	float shininess_strength = 1.0f;
	bool isbackface = false;

	aimaterial->Get(AI_MATKEY_COLOR_AMBIENT, ka_color);
	aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, kd_color);
	aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, ks_color);
	aimaterial->Get(AI_MATKEY_OPACITY, transparent);
	aimaterial->Get(AI_MATKEY_SHININESS, shininess);
	aimaterial->Get(AI_MATKEY_TWOSIDED, isbackface);
	aimaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength);

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
	mShininess = shininess;
	mTransparent = transparent;
	mIsBackFace = isbackface;
}


QHMaterial::~QHMaterial()
{
}
