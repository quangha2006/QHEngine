#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
enum TextureType
{
	TextureType_DIFFUSE,
	TextureType_SPECULAR,
	TextureType_HEIGHT,
	TextureType_AMBIENT
};
struct Texture {
	unsigned int id;
	TextureType type;
	std::string path;
};
class QHTexture
{
public:
	QHTexture();
	~QHTexture();
};

