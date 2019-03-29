#include "stdafx.h"
#include "ShaderManager.h"
#include "Utils.h"
ShaderManager *ShaderManager::instance = NULL;

GLuint ShaderManager::createProgram(const char * vtxSrc, const char * fragSrc, const char* definecode)
{
	GLint linked = GL_FALSE;
	GLuint vertexShader = createShader(GL_VERTEX_SHADER, vtxSrc, definecode);
	if (!vertexShader)
	{
		return 0;
	}
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragSrc, definecode);
	if (!fragmentShader)
	{
		return 0;
	}
	GLuint program = glCreateProgram();
	if (!program) {
		return 0;
	}
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		LOGE("Could not link program\n");
		GLint infoLogLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen) {
			GLchar* infoLog = (GLchar*)malloc(infoLogLen);
			if (infoLog) {
				glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
				LOGE("Could not link program:\n%s\n", infoLog);
				free(infoLog);
			}
		}
		glDeleteProgram(program);
		program = 0;
		return program;
	}

	return program;
}

GLuint ShaderManager::createShader(GLenum shaderType, const char * src, const char* definecode)
{
	string fullPath(Utils::getResourcesFolder() + src);

	LOGI("Load shader: %s %s\n", fullPath.c_str(), (definecode != NULL ? definecode : " "));
	FILE * pf = fopen(fullPath.c_str(), "rb");
	if (pf == NULL)
	{
		LOGE("Load shader ERROR: %s\n", fullPath.c_str());
		return false;
	}
	fseek(pf, 0, SEEK_END);
	long size = ftell(pf);
	fseek(pf, 0, SEEK_SET);

	char * shaderSrc = new char[size + 1];
	fread(shaderSrc, sizeof(char), size, pf);
	shaderSrc[size] = 0;
	fclose(pf);

	string final_shaderSrc =  Utils::getDefineVersionShader();
	if (definecode != NULL)
	{
		final_shaderSrc += '\n';
		final_shaderSrc += definecode;
		final_shaderSrc += '\n';
	}
	final_shaderSrc += string(shaderSrc);
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) {
		return 0;
	}

	const char *source = (const char *)final_shaderSrc.c_str();
	glShaderSource(shader, 1, &source, NULL);
	delete[] shaderSrc;

	GLint compiled = GL_FALSE;

	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLogLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen > 0) {
			GLchar* infoLog = (GLchar*)malloc(infoLogLen);
			if (infoLog) {
				glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
				LOGE("ERROR!\n");
				LOGE("Could not compile %s shader:\n%s\n", shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
				free(infoLog);
			}
		}
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

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

void ShaderManager::setBool(const std::string &name, bool value)
{
	glUniform1i(glGetUniformLocation(Current_program, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void ShaderManager::setInt(const std::string &name, int value)
{
	glUniform1i(glGetUniformLocation(Current_program, name.c_str()), value);
}
// ------------------------------------------------------------------------
void ShaderManager::setFloat(const std::string &name, float value)
{
	glUniform1f(glGetUniformLocation(Current_program, name.c_str()), value);
}
void ShaderManager::setFloat(const std::string &name, float value[], int size)
{
	glUniform1fv(glGetUniformLocation(Current_program, name.c_str()), size , &value[0]);
}
// ------------------------------------------------------------------------
void ShaderManager::setVec2(const std::string &name, const glm::vec2 *value, int size)
{
	glUniform2fv(glGetUniformLocation(Current_program, name.c_str()), size, &value[0].x);
}
void ShaderManager::setVec2(const std::string &name, const glm::vec2 &value)
{
	glUniform2fv(glGetUniformLocation(Current_program, name.c_str()), 1, &value[0]);
}
void ShaderManager::setVec2(const std::string &name, float x, float y)
{
	glUniform2f(glGetUniformLocation(Current_program, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void ShaderManager::setVec3(const std::string &name, const glm::vec3 &value)
{
	glUniform3fv(glGetUniformLocation(Current_program, name.c_str()), 1, &value[0]);
}
void ShaderManager::setVec3(const std::string &name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(Current_program, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void ShaderManager::setVec4(const std::string &name, const glm::vec4 &value)
{
	glUniform4fv(glGetUniformLocation(Current_program, name.c_str()), 1, &value[0]);
}
void ShaderManager::setVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(Current_program, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void ShaderManager::setMat2(const std::string &name, const glm::mat2 &mat)
{
	glUniformMatrix2fv(glGetUniformLocation(Current_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ShaderManager::setMat3(const std::string &name, const glm::mat3 &mat)
{
	glUniformMatrix3fv(glGetUniformLocation(Current_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ShaderManager::setMat4(const std::string &name, const glm::mat4 &mat)
{
	glUniformMatrix4fv(glGetUniformLocation(Current_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void ShaderManager::setBoneMat4(const std::string &name, vector<glm::mat4> mat)
{
	int m_boneLocation = glGetUniformLocation(ShaderManager::getInstance()->GetCurrentProgram(), name.c_str());
	if (m_boneLocation >= 0)
		glUniformMatrix4fv(m_boneLocation, mat.size(), GL_TRUE, glm::value_ptr(mat[0][0]));
}
