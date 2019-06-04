#include "Utils.h"
#include "Timer.h"
#include <cstdlib>
#include <ctime>

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
		std::string str("./Resources");

		if (!dirExists(str))
			str = "../../Resources/";

		return std::move(str);
	}
	std::string getDefineVersionShader()
	{
		std::string shaderversion("#version 330\n");

		return std::move(shaderversion);
	}
#else // defined(_WINDOWS)
	std::string getResourcesFolder()
	{
		std::string str("/sdcard/Android/data/com.android.learnning3D/files/"); // Need call java to get package name
	}
	std::string getDefineVersionShader()
	{
		std::string shaderversion("#version 300 es\n");

		return std::move(shaderversion);
	}
#endif

	void PrintMat4(glm::mat4 mat4)
	{
		LOGI("\n");
		LOGI("\n%f, %f, %f, %f", mat4[0][0], mat4[0][1], mat4[0][2], mat4[0][3]);
		LOGI("\n%f, %f, %f, %f", mat4[1][0], mat4[1][1], mat4[1][2], mat4[1][3]);
		LOGI("\n%f, %f, %f, %f", mat4[2][0], mat4[2][1], mat4[2][2], mat4[2][3]);
		LOGI("\n%f, %f, %f, %f\n", mat4[3][0], mat4[3][1], mat4[3][2], mat4[3][3]);
	}
	glm::vec3 RandomColor()
	{
		float r = 0.1;
		float g = 0.1;
		float b = 0.1;

		srand(Timer::GetTicks());

		r = (float)((rand() % 100) + 1) / 100.0f;

		g = (float)((rand() % 100) + 1) / 100.0f;

		b = (float)((rand() % 100) + 1) / 100.0f;

		return glm::vec3(r,g,b);
	}
}