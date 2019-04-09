#pragma once
#include <iostream>
#include <string>
#include <map>
#include "AppContext.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
private:
	GLuint createShader(GLenum shaderType, const char* src, bool isFromString = false, const char* definecode = NULL);
	std::map<char*, int> uniformLocations;
public:
	GLuint program;
	GLint position_Attribute;
	GLint normal_Attribute;
	GLint color_Attribute;
	GLint TexCoord_Attribute;
	GLint Tangent_Attribute;
	GLint Bitangent_Attribute;
	GLint Weights_Attribute;
	GLint IDs_Attribute;
	std::string shaderName;

	bool m_initialized;
	bool createProgram(const char* vtxSrc, const char* fragSrc, bool isFromString = false, const char* definecode = NULL);
	bool LoadShader(const char* fileVertexShader, const char* fileFragmentShader, bool isFromString = false, const char* definecode = NULL);
	void use();
	GLint getPosAttribute();
	GLint getTexCoodAttribute();
	GLint getColorAttribute();
	void setInt(const std::string &name, int value);
	void setMat4(const std::string &name, const glm::mat4 &mat);
	Shader();
	~Shader();
};

