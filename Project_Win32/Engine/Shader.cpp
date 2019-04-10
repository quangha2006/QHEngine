#include "Shader.h"
#include "Logs.h"
#include "Utils.h"

bool Shader::createProgram(const char * vtxSrc, const char * fragSrc, bool isFromString , const char* definecode)
{
	GLint linked = GL_FALSE;
	GLuint vertexShader = createShader(GL_VERTEX_SHADER, vtxSrc, isFromString, definecode);
	if (!vertexShader)
	{
		return false;
	}
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragSrc, isFromString, definecode);
	if (!fragmentShader)
	{
		return false;
	}
	program = glCreateProgram();
	if (!program) {
		return false;
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
		program = -1;
		return false;
	}

	return true;
}

GLuint Shader::createShader(GLenum shaderType, const char * src, bool isFromString , const char* definecode)
{
	char * shaderSrc = NULL;
	std::string final_shaderSrc = "";

	if (!isFromString)
	{
		std::string path_modif(Utils::getResourcesFolder() + src);

		LOGI("Load shader: %s %s\n", path_modif.c_str(), (definecode != NULL ? definecode : " "));

		FILE * pf = fopen(path_modif.c_str(), "rb");
		if (pf == NULL)
		{
			LOGE("Could not open: %s \n", path_modif.c_str());
			return false;
		}
		fseek(pf, 0, SEEK_END);
		long size = ftell(pf);
		fseek(pf, 0, SEEK_SET);

		shaderSrc = new char[size + 1];
		fread(shaderSrc, sizeof(char), size, pf);
		shaderSrc[size] = 0;
		fclose(pf);
	}
	else
	{
		shaderSrc = new char[strlen(src) + 1];
		strcpy(shaderSrc, src);
	}
		
	if (shaderSrc != NULL && strncmp(shaderSrc, "#version", 8) != 0)
	{
		final_shaderSrc =  Utils::getDefineVersionShader();
	}
	if (definecode != NULL)
	{
		final_shaderSrc += '\n';
		final_shaderSrc += definecode;
		final_shaderSrc += '\n';
	}
	GLuint shader = glCreateShader(shaderType);

	final_shaderSrc += std::string(shaderSrc);

	delete[] shaderSrc;
	if (shader == 0) {
		return 0;
	}
	const char *source = (const char *)final_shaderSrc.c_str();
	glShaderSource(shader, 1, &source, NULL);

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
				LOGE("Could not compile %s shader: %s\n", shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
				free(infoLog);
			}
		}
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

bool Shader::LoadShader(const char * fileVertexShader, const char * fileFragmentShader, bool isFromString , const char* definecode)
{
	if (!createProgram(fileVertexShader, fileFragmentShader, isFromString, definecode))
		return false;

	position_Attribute = glGetAttribLocation(program, "aPos");
	normal_Attribute = glGetAttribLocation(program, "aNormal");
	color_Attribute = glGetAttribLocation(program, "aColor");
	TexCoord_Attribute = glGetAttribLocation(program, "aTexCoords");
	Tangent_Attribute = glGetAttribLocation(program, "aTangent");
	Bitangent_Attribute = glGetAttribLocation(program, "aBitangent");
	Weights_Attribute = glGetAttribLocation(program, "sWeights");
	IDs_Attribute = glGetAttribLocation(program, "sIDs");
	m_initialized = true;
	return true;
}

void Shader::use()
{
	if (m_initialized)
		glUseProgram(program);
	else
		LOGE("ERROR! Cannot set UseProgram!");
}

GLint Shader::getPosAttribute()
{
	return position_Attribute;
}

GLint Shader::getTexCoodAttribute()
{
	return TexCoord_Attribute;
}

GLint Shader::getColorAttribute()
{
	return color_Attribute;
}

void Shader::setInt(const std::string & name, int value)
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setMat4(const std::string & name, const glm::mat4 & mat)
{
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

Shader::Shader()
	: position_Attribute(-1)
	, normal_Attribute(-1)
	, color_Attribute(-1)
	, TexCoord_Attribute(-1)
	, Tangent_Attribute(-1)
	, Bitangent_Attribute(-1)
	, Weights_Attribute(-1)
	, IDs_Attribute(-1)
	, program(-1)
	, m_initialized(false)
{
}

Shader::~Shader()
{
}
