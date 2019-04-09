#include "stdafx.h"
#include "ShaderManager.h"
#include "Utils.h"
ShaderManager *ShaderManager::instance = NULL;

ShaderManager * ShaderManager::getInstance()
{
	if (instance == NULL)
		instance = new ShaderManager();
	return instance;
}

bool ShaderManager::setUseProgram(const char * shadername)
{
	int shader_list_size = Shader_list.size();
	if (shader_list_size == 0 || !shadername) return false;
	for (int i = 0; i < shader_list_size; i++)
	{
		if (Shader_list[i]->shaderName.compare(shadername) == 0)
		{
			glUseProgram(Shader_list[i]->program);
			Current_program = Shader_list[i]->program;
			Current_shader = i;
			return true;
		}
	}
	LOGI("ERROR!: not found shader name: %s\n", shadername);
	return false;
}

bool ShaderManager::setUseProgram(GLuint shaderProgram)
{
	int shader_list_size = Shader_list.size();
	if (shader_list_size == 0 || shaderProgram == -1) return false;
	for (int i = 0; i < shader_list_size; i++)
	{
		if (Shader_list[i]->program == shaderProgram)
		{
			glUseProgram(Shader_list[i]->program);
			Current_program = Shader_list[i]->program;
			Current_shader = i;
			return true;
		}
	}
	LOGI("ERROR!: not found shader program: %d\n", shaderProgram);
	return false;
}

GLuint ShaderManager::GetProgram(const char * shadername)
{
	int shader_list_size = Shader_list.size();
	if (shader_list_size == 0 || !shadername) return false;
	for (int i = 0; i < shader_list_size; i++)
	{
		if (Shader_list[i]->shaderName.compare(shadername) == 0)
		{
			return Shader_list[i]->program;
		}
	}
	return 0;
}

GLuint ShaderManager::GetCurrentProgram()
{
	return Current_program;
}

Shader * ShaderManager::GetShader(const char * shadername)
{
	int shader_list_size = Shader_list.size();
	if (shader_list_size == 0 || !shadername) return nullptr;
	for (int i = 0; i < shader_list_size; i++)
	{
		if (Shader_list[i]->shaderName.compare(shadername) == 0)
		{
			return Shader_list[i];
		}
	}
	return nullptr;
}

Shader * ShaderManager::GetCurrentShader()
{
	return Shader_list[Current_shader];
}

bool ShaderManager::LoadFromString(const char * shadername, const char * fileVertexShader, const char * fileFragmentShader, const char* definecode)
{	
	Shader *shader = new Shader();
	if (!shader->LoadShader(fileVertexShader, fileFragmentShader, true, definecode))
		return false;

	shader->shaderName = string(shadername);

	Shader_list.push_back(shader);

	return true;
}

bool ShaderManager::LoadFromFile(const char * shadername, const char * fileVertexShader, const char * fileFragmentShader, const char* definecode)
{
	Shader *shader = new Shader();
	if (!shader->LoadShader(fileVertexShader, fileFragmentShader, false, definecode))
		return false;

	shader->shaderName = string(shadername);

	Shader_list.push_back(shader);

	return true;
}

ShaderManager::ShaderManager()
{
	Current_program = -1;
	Current_shader = -1;
}


ShaderManager::~ShaderManager()
{
	int shader_list_size = Shader_list.size();
	if (shader_list_size == 0) return;
	for (int i = 0; i < shader_list_size; i++)
	{
		glDeleteProgram(Shader_list[i]->program);
	}
	for (int i = 0; i < shader_list_size; i++)
	{
		delete Shader_list[i];
	}
}

void ShaderSet::setBool(const std::string &name, bool value)
{
	glUniform1i(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void ShaderSet::setInt(const std::string &name, int value)
{
	glUniform1i(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), value);
}
// ------------------------------------------------------------------------
void ShaderSet::setFloat(const std::string &name, float value)
{
	glUniform1f(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), value);
}
void ShaderSet::setFloat(const std::string &name, float value[], int size)
{
	glUniform1fv(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), size , &value[0]);
}
// ------------------------------------------------------------------------
void ShaderSet::setVec2(const std::string &name, const glm::vec2 *value, int size)
{
	glUniform2fv(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), size, &value[0].x);
}
void ShaderSet::setVec2(const std::string &name, const glm::vec2 &value)
{
	glUniform2fv(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), 1, &value[0]);
}
void ShaderSet::setVec2(const std::string &name, float x, float y)
{
	glUniform2f(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void ShaderSet::setVec3(const std::string &name, const glm::vec3 &value)
{
	glUniform3fv(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), 1, &value[0]);
}
void ShaderSet::setVec3(const std::string &name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void ShaderSet::setVec4(const std::string &name, const glm::vec4 &value)
{
	glUniform4fv(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), 1, &value[0]);
}
void ShaderSet::setVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void ShaderSet::setMat2(const std::string &name, const glm::mat2 &mat)
{
	glUniformMatrix2fv(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ShaderSet::setMat3(const std::string &name, const glm::mat3 &mat)
{
	glUniformMatrix3fv(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ShaderSet::setMat4(const std::string &name, const glm::mat4 &mat)
{
	glUniformMatrix4fv(glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void ShaderSet::setBoneMat4(const std::string &name, const vector<glm::mat4> &mat)
{
	int m_boneLocation = glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str());
	if (m_boneLocation >= 0)
		glUniformMatrix4fv(m_boneLocation, mat.size(), GL_TRUE, glm::value_ptr(mat[0][0]));
}