#include "stdafx.h"
#include "ShaderManager.h"
#include "Utils.h"
ShaderManager *ShaderManager::instance = NULL;

GLuint ShaderManager::createProgram(const char * vtxSrc, const char * fragSrc)
{
	GLint linked = GL_FALSE;
	GLuint vertexShader = createShader(GL_VERTEX_SHADER, vtxSrc);
	if (!vertexShader)
	{
		return 0;
	}
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
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
	LOGI("Create Program: %d\n", program);
	return program;
}

GLuint ShaderManager::createShader(GLenum shaderType, const char * src)
{
	string fullPath(Utils::getResourcesFolder() + src);

	LOGI("Create shader: %s\n", fullPath.c_str());
	FILE * pf = fopen(fullPath.c_str(), "rb");
	if (pf == NULL)
	{
		LOGE("Load %s failed\n", fullPath.c_str());
		return false;
	}
	fseek(pf, 0, SEEK_END);
	long size = ftell(pf);
	fseek(pf, 0, SEEK_SET);

	char * shaderSrc = new char[size + 1];
	fread(shaderSrc, sizeof(char), size, pf);
	shaderSrc[size] = 0;
	fclose(pf);

	string final_shaderSrc;
#ifdef ANDROID
	final_shaderSrc = "#version 300 es\n";
#else
	final_shaderSrc = "#version 330\n";
#endif
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
	//LOGI("DONE! (id: %d)\n", shader);
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

Shaderv2 * ShaderManager::GetShader(const char * shadername)
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

Shaderv2 * ShaderManager::GetCurrentShader()
{
	return Shader_list[Current_shader];
}

bool ShaderManager::Init(const char * shadername, const char * fileVertexShader, const char * fileFragmentShader)
{
	LOGI("\n");
	GLuint program = createProgram(fileVertexShader, fileFragmentShader);
	if (program == 0)
		return false;
	Shaderv2 *shader = new Shaderv2();
	shader->shaderName = string(shadername);
	shader->program = program;
	//finding location of uniforms / attributes
	shader->position_Attribute = glGetAttribLocation(program, "aPos");
	shader->TexCoord_Attribute = glGetAttribLocation(program, "aTexCoords");
	shader->Weights_Attribute = glGetAttribLocation(program, "sWeights");
	shader->IDs_Attribute = glGetAttribLocation(program, "sIDs");
	shader->normal_Attribute = glGetAttribLocation(program, "aNormal");
	shader->Tangent_Attribute = glGetAttribLocation(program, "aTangent");
	shader->Bitangent_Attribute = glGetAttribLocation(program, "aBitangent");
	Shader_list.push_back(shader);

	return true;
}

ShaderManager::ShaderManager()
{
	Current_program = 0;
	Current_shader = 0;
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
