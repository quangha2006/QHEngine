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
struct ETC2Header
{
	unsigned char paddedWidthMSB;
	unsigned char paddedWidthLSB;
	unsigned char paddedHeightMSB;
	unsigned char paddedHeightLSB;
	unsigned char widthMSB;
	unsigned char widthLSB;
	unsigned char heightMSB;
	unsigned char heightLSB;
	ETC2Header(unsigned char *data)
	{
		paddedWidthMSB = data[8];
		paddedWidthLSB = data[9];
		paddedHeightMSB = data[10];
		paddedHeightLSB = data[11];
		widthMSB = data[12];
		widthLSB = data[13];
		heightMSB = data[14];
		heightLSB = data[15];
	}
};
struct Texture {
	GLuint id;
	GLint width;
	GLint height;
	aiTextureType type;
	std::string path;
};
namespace QHTexture
{
	Texture TextureFromFile(const char * path, const std::string & directory, aiTextureType type = aiTextureType::aiTextureType_UNKNOWN, GLuint textureID = 0, bool gammaCorrection = false);
	GLuint loadCubemap(const char * texturepath, std::vector<std::string> faces, bool gammaCorrection);
	GLuint GenTextureId();
};

#endif //!__QHTEXTURE_H__