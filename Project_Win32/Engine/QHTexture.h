#ifndef __QHTEXTURE_H__
#define __QHTEXTURE_H__

#include "stdafx.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Texture {
	GLuint id;
	GLint width;
	GLint height;
	aiTextureType type;
	aiString path;
};
namespace QHTexture
{
	GLuint TextureFromFile(const char * path, const std::string & directory, GLint &width, GLint &height, GLuint textureID = 0, bool gammaCorrection = false);
	GLuint loadCubemap(const char * texturepath, std::vector<std::string> faces, bool gammaCorrection);
	GLuint GenTextureId();
};

#endif //!__QHTEXTURE_H__