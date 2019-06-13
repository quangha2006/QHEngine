#include <jni.h>

#include <string>
#include "ScopeGetEnv.h"

namespace acp_utils
{
	class PackageUtils
	{
	private:
		static JavaVM* s_pVM;
		static jclass  s_LoadedJavaClass;
	public:
		static void SetJavaVM(JavaVM*);
		static void LoadClasses();
		static JavaVM*	GetJavaVm();
		static std::string getExternalStoragePublicDirectory();

		static void DestroyJavaVM();
		static void DeleteGlobalJavaClass();
	};
	JavaVM* GetVM();
}
