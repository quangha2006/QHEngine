#ifndef __SHADER_H__
#define __SHADER_H__

#include <iostream>
#include <string>
#include <map>
#include "AppContext.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
///////////////////
//- highp for vertex positions,
//- mediump for texture coordinates,
//- lowp for colors.
//////////////////
class Shader
{
private:
	GLuint createShader(GLenum shaderType, const char* src, bool isFromString = false, const char* definecode = NULL);
	std::map<char*, int> uniformLocations;
	GLuint mVertexShader;
	GLuint mFragmentShader;
	GLint GetLocation(const char* name);
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

	void setInt(const char* name, int value);
	void setFloat(const char* name, float value[], int size);
	void setFloat(const char* name, float value);
	void setVec2(const char* name, const glm::vec2 *value, int size);
	void setVec2(const char* name, const glm::vec2 &value);
	void setVec2(const char* name, float x, float y);
	void setVec3(const char* name, const glm::vec3 &value);
	void setVec3(const char* name, float x, float y, float z);
	void setVec4(const char* name, const glm::vec4 &value);
	void setVec4(const char* name, float x, float y, float z, float w);
	void setMat2(const char* name, const glm::mat2 &mat);
	void setMat3(const char* name, const glm::mat3 &mat);
	void setMat4(const char* name, const glm::mat4 &mat);
	void setBoneMat4(const char* name, const vector<glm::mat4> &mat);

	Shader();
	~Shader();
};

#endif //!__SHADER_H__