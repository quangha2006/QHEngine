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
	unsigned int id;
	int width;
	int height;
	aiTextureType type;
	aiString path;
};
namespace QHTexture
{
	GLuint TextureFromFile(const char * path, const std::string & directory, int &width, int &height, GLuint textureID = -1, bool gammaCorrection = false);
	GLuint loadCubemap(const char * texturepath, std::vector<std::string> faces, bool gammaCorrection);
	unsigned int GenTextureId();
};

#endif //!__QHTEXTURE_H__