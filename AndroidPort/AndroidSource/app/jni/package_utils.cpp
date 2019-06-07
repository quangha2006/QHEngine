#include "package_utils.h"
#include <algorithm>
#include <cmath>
#include <string>

#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>

namespace acp_utils
{
	std::string getExternalStoragePublicDirectory()
	{
		JNIEnv* pEnv = NULL;
		ScopeGetEnv sta(pEnv);

		jmethodID mGetExternalStorage = pEnv->GetStaticMethodID(GetClass("GLES3View"), "GetExternalStorage", "()V");

		jstring fullpathString = (jstring)pEnv->CallStaticObjectMethod(GetClass("GLES3View"), mGetExternalStorage);

		const char* fullpath = pEnv->GetStringUTFChars(fullpathString, NULL);
		std::string result = fullpath;

		pEnv->ReleaseStringUTFChars(fullpathString, fullpath);
		pEnv->DeleteLocalRef(fullpathString);

		return result;
	}
};//end namespace acp_utils
