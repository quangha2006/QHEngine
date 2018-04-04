#include "Shader.h"
#include "Logs.h"


bool Shader::createProgram(const char * vtxSrc, const char * fragSrc, bool isFromString )
{
	GLint linked = GL_FALSE;
	GLuint vertexShader = createShader(GL_VERTEX_SHADER, vtxSrc);
	if (!vertexShader)
	{
		return false;
	}
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
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
		program = 0;
		return false;
	}
	LOGI("Create Program: %d\n", program);
	return true;
}

GLuint Shader::createShader(GLenum shaderType, const char * src, bool isFromString )
{
	LOGI("Create shader: %s\n", src);
	char * shaderSrc;
	if (!isFromString)
	{
		std::string path_modif(src);
#ifdef ANDROID
		if (path_modif.find("../") == 0)
		{
			path_modif.replace(0, 3, "/sdcard/");
		}
#endif
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

	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) {
		return 0;
	}
	if (!isFromString)
	{
		glShaderSource(shader, 1, (const char**)&shaderSrc, NULL);
		delete[] shaderSrc;
	}
	else
		glShaderSource(shader, 1, (const char**)&src, NULL);

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

bool Shader::LoadShader(const char * fileVertexShader, const char * fileFragmentShader, bool isFromString )
{
	if (!createProgram(fileVertexShader, fileFragmentShader, isFromString))
		return false;
	position_Attribute = glGetAttribLocation(program, "aPos");
	color_Attribute = glGetAttribLocation(program, "aColor");
	return true;
}

Shader::Shader()
{
}


Shader::~Shader()
{
}
