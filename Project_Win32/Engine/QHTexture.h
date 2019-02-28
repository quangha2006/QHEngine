#pragma once
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
//#include <SOIL.h>

//enum TextureType
//{
//	TextureType_DIFFUSE,
//	TextureType_SPECULAR,
//	TextureType_NORMALS,
//	TextureType_HEIGHT,
//	TextureType_AMBIENT
//};
struct Texture {
	unsigned int id;
	aiTextureType type;
	aiString path;
};
namespace QHTexture
{
	GLuint TextureFromFile(const char * path, const std::string & directory, GLuint textureID = -1, bool gammaCorrection = false);
	GLuint loadCubemap(const char * texturepath, std::vector<std::string> faces, bool gammaCorrection);
	unsigned int GenTextureId();
};

