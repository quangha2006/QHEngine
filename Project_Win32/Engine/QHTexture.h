#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <assimp/Importer.hpp>
enum TextureType
{
	TextureType_DIFFUSE,
	TextureType_SPECULAR,
	TextureType_NORMALS,
	TextureType_HEIGHT,
	TextureType_AMBIENT
};
struct Texture {
	unsigned int id;
	TextureType type;
	aiString path;
};
class QHTexture
{
public:
	QHTexture();
	~QHTexture();
};

