#pragma once
#include "stdafx.h"
#include <vector>
// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
using namespace std;
struct Shaderv2
{
	GLuint program;
	GLint position_Attribute;
	GLint normal_Attribute;
	GLint color_Attribute;
	GLint TexCoord_Attribute;
	GLint Tangent_Attribute;
	GLint Bitangent_Attribute;
	GLint Weights_Attribute;
	GLint IDs_Attribute;
	string shaderName;
	Shaderv2()
	{
		position_Attribute = -1;
		normal_Attribute = -1;
		color_Attribute = -1;
		TexCoord_Attribute = -1;
		Tangent_Attribute = -1;
		Bitangent_Attribute = -1;
		Weights_Attribute = -1;
		IDs_Attribute = -1;
	}
};
class ShaderManager
{
private:
	static ShaderManager *instance;
	vector<Shaderv2*> Shader_list;
	GLuint Current_program;
	GLuint Current_shader;
	GLuint createProgram(const char* vtxSrc, const char* fragSrc);
	GLuint createShader(GLenum shaderType, const char* src);
public:
	static ShaderManager *getInstance();
	bool setUseProgram(const char *shadername);
	GLuint GetProgram(const char *shadername);
	GLuint GetCurrentProgram();
	Shaderv2 *GetShader(const char *shadername);
	Shaderv2 *GetCurrentShader();
	bool Init(const char* shadername,const char* fileVertexShader, const char* fileFragmentShader);
	void setBool(const std::string &name, bool value);
	void setInt(const std::string &name, int value);
	void setFloat(const std::string &name, float value[], int size);
	void setFloat(const std::string &name, float value);
	void setVec2(const std::string &name, const glm::vec2 *value, int size);
	void setVec2(const std::string &name, const glm::vec2 &value);
	void setVec2(const std::string &name, float x, float y);
	void setVec3(const std::string &name, const glm::vec3 &value);
	void setVec3(const std::string &name, float x, float y, float z);
	void setVec4(const std::string &name, const glm::vec4 &value);
	void setVec4(const std::string &name, float x, float y, float z, float w);
	void setMat2(const std::string &name, const glm::mat2 &mat);
	void setMat3(const std::string &name, const glm::mat3 &mat);
	void setMat4(const std::string &name, const glm::mat4 &mat);
	void setBoneMat4(const std::string &name, vector<glm::mat4> mat);
	ShaderManager();
	~ShaderManager();
};