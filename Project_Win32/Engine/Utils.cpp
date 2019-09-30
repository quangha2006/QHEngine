#include "Utils.h"
#include "Timer.h"
#include <cstdlib>
#include <ctime>

#if defined(ANDROID)
#include "package_utils.h"
#endif

#if defined(_WINDOWS)
bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}
#endif

namespace Utils
{
#if defined(_WINDOWS)
	std::string getResourcesFolder()
	{
		std::string str_path("./Resources/");

		if (!dirExists(str_path))
			str_path = "../../Resources/";

		return str_path;
	}
	std::string getDefineVersionShader()
	{
		return "#version 330\n";
	}
#elif defined(ANDROID) // defined(_WINDOWS)
	std::string getResourcesFolder()
	{
		static std::string str_path = acp_utils::PackageUtils::getExternalStoragePublicDirectory() + '/';
		return str_path;
	}
	std::string getDefineVersionShader()
	{
		return "#version 300 es\n";
	}
#endif

	void PrintMat4(const glm::mat4 &mat4)
	{
		LOGI("\n");
		LOGI("\n%f, %f, %f, %f", mat4[0][0], mat4[0][1], mat4[0][2], mat4[0][3]);
		LOGI("\n%f, %f, %f, %f", mat4[1][0], mat4[1][1], mat4[1][2], mat4[1][3]);
		LOGI("\n%f, %f, %f, %f", mat4[2][0], mat4[2][1], mat4[2][2], mat4[2][3]);
		LOGI("\n%f, %f, %f, %f\n", mat4[3][0], mat4[3][1], mat4[3][2], mat4[3][3]);
	}
	//void PrintVec3(aiVec)
	//{

	//}
	void PrintGLEXTENSIONS()
	{
		GLint n = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);
		//PFNGLGETSTRINGIPROC glGetStringi = 0;
		//glGetStringi = (PFNGLGETSTRINGIPROC)wglGetProcAddress("glGetStringi");

		for (GLint i = 0; i < n; i++)
		{
			const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
			LOGI("Ext %d: %s\n", i, extension);
		}
	}
	bool IsExtensionSupported(const char *name)
	{
		GLint n = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);
		for (GLint i = 0; i < n; i++)
		{
			const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
			if (strstr(extension, name))
			{
				return true;
			}
		}
		return false;
	}
	glm::vec3 RandomColor()
	{
		float r = 0.1f;
		float g = 0.1f;
		float b = 0.1f;

		srand((unsigned int)Timer::GetTicks());

		r = (float)((rand() % 100) + 1) / 100.0f;

		g = (float)((rand() % 100) + 1) / 100.0f;

		b = (float)((rand() % 100) + 1) / 100.0f;

		return glm::vec3(r,g,b);
	}
}