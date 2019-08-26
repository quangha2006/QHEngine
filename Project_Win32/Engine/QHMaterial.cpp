#include "QHMaterial.h"
#include "ShaderManager.h"
#include "RenderManager.h"
#include "Debugging.h"

void QHMaterial::Apply(RenderTargetType RT_Type, bool isEnableAlpha)
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;
	unsigned int texture_actived = 0;

	ShaderSet::setBool("useNormalMap", false);

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
}

void QHMaterial::Draw()
{
	if (mIndices_size <= 0) return;

	QHEngine::DrawElements(GL_TRIANGLES, mIndices_size, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * mIndices_index));
}

QHMaterial::QHMaterial()
	: mHasNormals(true)
	, mIndices_index(0)
	, mIndices_size(0)
{
}


QHMaterial::~QHMaterial()
{
}
