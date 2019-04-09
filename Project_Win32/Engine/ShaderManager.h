#pragma once
#include "stdafx.h"
#include <vector>
// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "Shader.h"
using namespace std;

class ShaderManager
{
private:
	static ShaderManager *instance;
	vector<Shader*> Shader_list;
	GLuint Current_program;
	GLuint Current_shader;
public:
	static ShaderManager *getInstance();
	bool setUseProgram(const char *shadername);
	bool setUseProgram(GLuint shaderProgram);
	GLuint GetProgram(const char *shadername);
	GLuint GetCurrentProgram();
	Shader *GetShader(const char *shadername);
	Shader *GetCurrentShader();
	bool LoadFromFile(const char* shadername,const char* fileVertexShader, const char* fileFragmentShader, const char* definecode = NULL);
	bool LoadFromString(const char* shadername, const char* fileVertexShader, const char* fileFragmentShader, const char* definecode = NULL);

	ShaderManager();
	~ShaderManager();
};

namespace ShaderSet
{
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
	void setBoneMat4(const std::string &name,const vector<glm::mat4> &mat);
}