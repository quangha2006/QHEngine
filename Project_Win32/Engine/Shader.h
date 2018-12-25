#pragma once
#include <iostream>
#include <string>
#include "AppContext.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
private:
	GLuint program;
	GLint position_Attribute;
	GLint texCoord_Attribute;
	GLint color_Attribute;
	GLuint createShader(GLenum shaderType, const char* src, bool isFromString = false);
	
public:
	bool m_initialized;
	bool createProgram(const char* vtxSrc, const char* fragSrc, bool isFromString = false);
	bool LoadShader(const char* fileVertexShader, const char* fileFragmentShader, bool isFromString = false);
	void use();
	GLint getPosAttribute();
	GLint getTexCoodAttribute();
	GLint getColorAttribute();
	//void setBool(const std::string &name, bool value);
	void setInt(const std::string &name, int value);
	//void setFloat(const std::string &name, float value);
	//void setVec2(const std::string &name, const glm::vec2 &value);
	//void setVec2(const std::string &name, float x, float y);
	//void setVec3(const std::string &name, const glm::vec3 &value);
	//void setVec3(const std::string &name, float x, float y, float z);
	//void setVec4(const std::string &name, const glm::vec4 &value);
	//void setVec4(const std::string &name, float x, float y, float z, float w);
	//void setMat2(const std::string &name, const glm::mat2 &mat);
	//void setMat3(const std::string &name, const glm::mat3 &mat);
	void setMat4(const std::string &name, const glm::mat4 &mat);
	Shader();
	~Shader();
};

