#include "Utils.h"
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
}