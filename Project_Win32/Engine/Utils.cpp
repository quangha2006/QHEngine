#include "Utils.h"
#include <cstdlib>
#include <ctime>
//#include "package_utils.h"
namespace Utils
{
	std::string getResourcesFolder()
	{
#if defined(_WINDOWS)
		std::string str("../../Resources/");
#else defined(ANDROID)
		std::string str("/sdcard/Android/data/com.android.learnning3D/files/");
#endif
		return std::move(str);
	}
	std::string getDefineVersionShader()
	{
#ifdef ANDROID
		return "#version 300 es\n";	//300 es
#else
		return "#version 330\n";
#endif
	}
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
		srand((int)time(0));
		float x = rand() % 99 + 1;
		float r = x / 100.0;
		//srand(time(NULL));
		float y = rand() % 99 + 1;
		float g = y / 100.0;
		//srand(time(NULL));
		float z = rand() % 99 + 1;
		float b = z / 100.0;
		LOGI("%f, %f, %f\n", x, y, z);
		return glm::vec3(r,g,b);
	}
}