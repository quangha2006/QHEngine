#include "package_utils.h"

namespace acp_utils
{
	JavaVM*	PackageUtils::s_pVM				= 0;
	jclass	PackageUtils::s_LoadedJavaClass = 0;

	std::string PackageUtils::getExternalStoragePublicDirectory()
	{
		JNIEnv* pEnv = NULL;

		ScopeGetEnv sta(pEnv);

		jmethodID mGetExternalStorage = pEnv->GetStaticMethodID(s_LoadedJavaClass, "getExternalStorage", "()Ljava/lang/String;");

		jstring fullpathString = (jstring)pEnv->CallStaticObjectMethod(s_LoadedJavaClass, mGetExternalStorage);

		const char* fullpath = pEnv->GetStringUTFChars(fullpathString, NULL);
		std::string result = fullpath;

		pEnv->ReleaseStringUTFChars(fullpathString, fullpath);
		pEnv->DeleteLocalRef(fullpathString);

		return result;
	}

	void PackageUtils::SetJavaVM(JavaVM* vm)
	{
		s_pVM = vm;
	}
	void PackageUtils::LoadClasses()
	{
		JNIEnv* pEnv = NULL;

		ScopeGetEnv sta(pEnv);

		jclass localClass = pEnv->FindClass("com/android/quanghaengine/GLES3View");

		s_LoadedJavaClass = reinterpret_cast<jclass>(pEnv->NewGlobalRef(localClass));
	}
	void PackageUtils::ExitApplication(bool restart)
	{
		JNIEnv* pEnv = NULL;
		ScopeGetEnv sta(pEnv);

		jmethodID exitApp = pEnv->GetStaticMethodID(s_LoadedJavaClass, "ExitApplication", "(Z)V");
		pEnv->CallStaticVoidMethod(s_LoadedJavaClass, exitApp, restart);
	}
	void PackageUtils::ShowToastMessage(const char* key, int duration)
	{
		JNIEnv* pEnv = NULL;
		ScopeGetEnv sta(pEnv);

		jstring jkey = pEnv->NewStringUTF(key);
			
		jmethodID showToastMessageID = pEnv->GetStaticMethodID(s_LoadedJavaClass, "ShowToastMessage", "(Ljava/lang/String;I)V");
		pEnv->CallStaticVoidMethod(s_LoadedJavaClass, showToastMessageID, jkey, duration);
		
		pEnv->DeleteLocalRef(jkey);
		
	}
	void PackageUtils::DestroyJavaVM()
	{
		s_pVM->DestroyJavaVM();
	}

	void PackageUtils::DeleteGlobalJavaClass()
	{
		JNIEnv* pEnv = NULL;

		ScopeGetEnv sta(pEnv);

		pEnv->DeleteGlobalRef(s_LoadedJavaClass);
	}

	JavaVM* PackageUtils::GetJavaVm()
	{
		return s_pVM;
	}

	JavaVM* GetVM()
	{
		return PackageUtils::GetJavaVm();
	}
};//end namespace acp_utils
