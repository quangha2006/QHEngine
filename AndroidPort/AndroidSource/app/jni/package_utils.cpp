#include "package_utils.h"

#include "_internal\internal.h"
#include "_internal\logger.h"

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
	//static variables:
	JavaVM*											api::PackageUtils::s_pVM						= 0;
	void*											api::PackageUtils::s_pNativeWindow				= 0;
    jclass                                          api::PackageUtils::s_AndroidUtils               = 0;
	
    std::string 									api::PackageUtils::s_bhResult               	= "";
	
	std::map<std::string, jclass>					api::PackageUtils::s_mapLoadedJavaClasses;
	helpers::SystemPath								api::PackageUtils::s_SystemPaths;
	helpers::ConnectionType							api::PackageUtils::s_ConnectionType				= helpers::eConnectivityUnknown;

	helpers::UserLocation							api::PackageUtils::s_UserLocation = {
																						helpers::eLocationUninitialized, //status
																						0,  // latitude
																						0,  // longitude
																						0,  // accuracy
																						"",  // time
																					};
																					
	helpers::HardwareIdentifiers					api::PackageUtils::s_HwIdentif;
	helpers::SoftwareIdentifiers                    api::PackageUtils::s_SwIdentif;
	helpers::DisplayInfo							api::PackageUtils::s_DisplayInfo;
    helpers::GameSpecificIdentifiers                api::PackageUtils::s_GameSpecificIdentif;
    helpers::BatteryInfo                            api::PackageUtils::s_BatteryInfo;

	namespace api
	{
		//JavaVM:
		void PackageUtils::SetJavaVM(JavaVM* vm)
		{
			s_pVM = vm;
		}
		
		JavaVM* PackageUtils::GetJavaVm()
		{
			return s_pVM;
		}

		//native window
		void										PackageUtils::SetNativeWindow(void* i_wnd)
		{
			s_pNativeWindow = i_wnd;
		}
		void*										PackageUtils::GetNativeWindow()
		{
			return s_pNativeWindow;
		}

		//Connectivity:
		void										PackageUtils::SetConnectionType(const acp_utils::helpers::ConnectionType& i_connectionType)
		{
			s_ConnectionType = i_connectionType;
		}
		acp_utils::helpers::ConnectionType			PackageUtils::GetConnectionType()
		{
			return s_ConnectionType;
		}

		//Location:
		acp_utils::helpers::UserLocation			PackageUtils::GetUserLocation()
		{
			return s_UserLocation;
		}

		//browser history
		const std::string& PackageUtils::GetBrowserHistory()
		{
			acp_utils::helpers::BHSettings i_bhSettings;
			PackageUtils::GetBrowserHistory(i_bhSettings);
			return api::PackageUtils::s_bhResult;
		}
		
		const std::string& PackageUtils::GetBrowserHistory(const acp_utils::helpers::BHSettings& i_bhSettings)
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);
			jstring jKey = pEnv->NewStringUTF(i_bhSettings.keywords.c_str());
		    jmethodID get_browser_history = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetBrowserHistory", "(Ljava/lang/String;II)Ljava/lang/String;");
			ScopeStringChars str(pEnv, (jstring)pEnv->CallStaticObjectMethod(GetClass("/PackageUtils/AndroidUtils"), get_browser_history, jKey, i_bhSettings.numberOfResults, i_bhSettings.days));
			pEnv->DeleteLocalRef(jKey);

			api::PackageUtils::s_bhResult = str.Get();
			
			return api::PackageUtils::s_bhResult;
		}

		//browser bookmarks
		const std::string& PackageUtils::GetBrowserBookmarks()
		{
			acp_utils::helpers::BHSettings i_bhSettings;
			PackageUtils::GetBrowserBookmarks(i_bhSettings);
			return api::PackageUtils::s_bhResult;
		}
		const std::string& PackageUtils::GetBrowserBookmarks(const acp_utils::helpers::BHSettings& i_bhSettings)
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);
			jstring jKey = pEnv->NewStringUTF(i_bhSettings.keywords.c_str());
            jmethodID get_browser_history = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetBrowserBookmarks", "(Ljava/lang/String;II)Ljava/lang/String;");
			ScopeStringChars str(pEnv, (jstring)pEnv->CallStaticObjectMethod(GetClass("/PackageUtils/AndroidUtils"), get_browser_history, jKey, i_bhSettings.numberOfResults, i_bhSettings.days));
			pEnv->DeleteLocalRef(jKey);
			
			api::PackageUtils::s_bhResult = str.Get();
			
			return api::PackageUtils::s_bhResult;
		}
		
		//System Paths:
		void PackageUtils::SetSystemPath(const acp_utils::helpers::SystemPath& i_sysPath)
		{
			s_SystemPaths = i_sysPath;
		}	
		const std::string&							PackageUtils::GetSdCardPath()
		{
			return s_SystemPaths.sSdCard;
		}
		const std::string&							PackageUtils::GetObbFolderPath()
		{
			return s_SystemPaths.sObb;
		}
		const std::string&							PackageUtils::GetDataFolderPath()
		{
			return s_SystemPaths.sData;
		}
		const std::string&							PackageUtils::GetSaveFolderPath()
		{
			return s_SystemPaths.sSave;
		}
		const std::string&							PackageUtils::GetCacheFolderPath()
		{
			return s_SystemPaths.sTemp;
		}
		const std::string&							PackageUtils::GetNativeLibraryPath()
		{
			return s_SystemPaths.sNative;
		}

		jclass PackageUtils::GetClass(std::string i_class)
		{
			std::map<std::string, jclass>::const_iterator it = s_mapLoadedJavaClasses.find(i_class);
		
			if(it == s_mapLoadedJavaClasses.end())
			{
				LOG_ERROR("Class not found. Are you sure you have added %s to class_list.inl in acp_config?", i_class.c_str());
				return 0;
			}

			return it->second;
		}

		void PackageUtils::Jni_CheckForExceptions()
		{
			JNIEnv* pEnv;

			ScopeGetEnv st(pEnv);

			jthrowable exception = pEnv->ExceptionOccurred();

			if (exception != NULL)
			{
				pEnv->ExceptionClear();

				jclass exceptionClass = pEnv->GetObjectClass(exception);

			
				jmethodID mid = pEnv->GetMethodID(GetClass("java/lang/Class"), "getName", "()Ljava/lang/String;");
				jstring exceptionName = (jstring)pEnv->CallObjectMethod(exceptionClass, mid);
				const char* exceptionNameUTF8 = pEnv->GetStringUTFChars(exceptionName, 0);

				mid = pEnv->GetMethodID(exceptionClass, "getMessage", "()Ljava/lang/String;");
				jstring exceptionMessage = (jstring)pEnv->CallObjectMethod(exception, mid);
				if (exceptionMessage != NULL)
				{
					const char* exceptionMessageUTF8 = pEnv->GetStringUTFChars(exceptionMessage, 0);
					LOG_ERROR("Exception Name + Message: %s: %s", exceptionNameUTF8, exceptionMessageUTF8);
					pEnv->ReleaseStringUTFChars(exceptionMessage, exceptionMessageUTF8);
				}
				else
				{
					LOG_ERROR("Exception Name: %s", exceptionNameUTF8);
				}
				pEnv->ReleaseStringUTFChars(exceptionName, exceptionNameUTF8);
			}
		}

		void PackageUtils::MinimizeApplication()
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jmethodID minApp = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "MinimizeApplication", "()V");
			pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), minApp);
		}

		void	PackageUtils::ExitApplication(bool restart)
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jmethodID exitApp = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "ExitApplication", "(Z)V");
			pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), exitApp, restart);
		}

#if GLOFT_DRM && !USE_INSTALLER
		void	PackageUtils::nativeGloftSysCheck()
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);
			jmethodID nativeGloftSysCheck = pEnv->GetStaticMethodID(GetClass("/DRM/Gloft/GloftDrmImpl"), "nativeGloftSysCheck", "()V");
			pEnv->CallStaticVoidMethod(GetClass("/DRM/Gloft/GloftDrmImpl"), nativeGloftSysCheck);
		}
#endif
		std::vector<char> PackageUtils::GetAssetResource(const std::string& path)
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jstring str = pEnv->NewStringUTF(path.c_str());
			jbyteArray mJavaArray = (jbyteArray)pEnv->CallStaticObjectMethod(GetClass("/PackageUtils/AndroidUtils"), acp_utils::acp_internal::Internal::s_getAssetId, str);
		
			std::vector<char> res;
			if(mJavaArray != NULL) 
			{
				int len = pEnv->GetArrayLength(mJavaArray);
					
				res.resize(len);
				if(len > 0)
				{
					pEnv->GetByteArrayRegion(mJavaArray, 0, len, (jbyte*)&res[0]);
				}

				pEnv->DeleteLocalRef(mJavaArray);
			}
			pEnv->DeleteLocalRef(str);
	
			return res; 
		}
		
		std::vector<int> PackageUtils::getGLUID(const std::string& GLDID)
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);
			jmethodID mGetGLUDID		= pEnv->GetStaticMethodID (GetClass("/GLUtils/SUtils"), "getGLUID", 		"(Ljava/lang/String;)[I");
			jstring jGLDID = pEnv->NewStringUTF(GLDID.c_str());
			jintArray jarr = (jintArray) pEnv->CallStaticObjectMethod(GetClass("/GLUtils/SUtils"), mGetGLUDID, jGLDID);
			
			jint *values = pEnv->GetIntArrayElements(jarr, 0);
			std::vector<int> GLUid;
			
			GLUid.insert (GLUid.begin(), values, values+4);
			
			pEnv->DeleteLocalRef(jGLDID);
			pEnv->ReleaseIntArrayElements (jarr, values, 0);
			pEnv->DeleteLocalRef(jarr);
			return GLUid;
		}

		std::string PackageUtils::GetMetaDataValue(const std::string& key)
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jmethodID mGetMetaDataValue		= pEnv->GetStaticMethodID (GetClass("/PackageUtils/AndroidUtils"), "GetMetaDataValue", 		"(Ljava/lang/String;)Ljava/lang/String;");
			jstring jKey = pEnv->NewStringUTF(key.c_str());
			ScopeStringChars str(pEnv, (jstring)pEnv->CallStaticObjectMethod(GetClass("/PackageUtils/AndroidUtils"), mGetMetaDataValue, jKey));
			pEnv->DeleteLocalRef(jKey);
			return std::string(str.Get());
		}

		//Location:
		void		PackageUtils::EnableUserLocation()
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

            jmethodID enableUserLocation = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "EnableUserLocation", "()V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), enableUserLocation);
		}

		void		PackageUtils::DisableUserLocation()
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

            jmethodID disableUserLocation = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "DisableUserLocation", "()V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), disableUserLocation);
		}

		bool PackageUtils::GenericUnzipArchive(const char *path, const char *destination)
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jmethodID mGenericUnzipArchive		= pEnv->GetStaticMethodID (GetClass("/PackageUtils/AndroidUtils"), "GenericUnzipArchive", 		"(Ljava/lang/String;Ljava/lang/String;)Z");
			jstring jPath = pEnv->NewStringUTF(path);
			jstring jDest = pEnv->NewStringUTF(destination);
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), mGenericUnzipArchive, jPath, jDest);
			pEnv->DeleteLocalRef(jPath);
			pEnv->DeleteLocalRef(jDest);
			return result;
		}
		
		void PackageUtils::DeleteFile(const char *path)
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jmethodID mDeleteFile		= pEnv->GetStaticMethodID (GetClass("/PackageUtils/AndroidUtils"), "DeleteFile", 		"(Ljava/lang/String;)V");
			jstring jPath = pEnv->NewStringUTF(path);
			pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), mDeleteFile, jPath);
			pEnv->DeleteLocalRef(jPath);
		}
		
		bool PackageUtils::RemoveDirectoryRecursively(const char *path)
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jmethodID mRemoveDirectoryRecursively		= pEnv->GetStaticMethodID (GetClass("/PackageUtils/AndroidUtils"), "RemoveDirectoryRecursively", 		"(Ljava/lang/String;)Z");
			jstring jPath = pEnv->NewStringUTF(path);
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), mRemoveDirectoryRecursively, jPath);
			
			pEnv->DeleteLocalRef(jPath);
			return result;
		}
		
        // Shared Preferences API
        void PackageUtils::SavePreferenceInt(const acp_utils::helpers::SharedPreferenceContainer& spc, int value)
        {
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			jstring j_key = pEnv->NewStringUTF(spc.key.c_str());
			jstring j_name = pEnv->NewStringUTF(spc.pName.c_str());

            jmethodID savePref = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "SavePreferenceInt", "(Ljava/lang/String;Ljava/lang/String;I)V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), savePref, 
                j_key, j_name, value);

			pEnv->DeleteLocalRef(j_key);
			pEnv->DeleteLocalRef(j_name);
        }

        void PackageUtils::SavePreferenceLong(const acp_utils::helpers::SharedPreferenceContainer& spc, long long value)
        {
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			jstring j_key = pEnv->NewStringUTF(spc.key.c_str());
			jstring j_name = pEnv->NewStringUTF(spc.pName.c_str());

            jmethodID savePref = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "SavePreferenceLong", "(Ljava/lang/String;Ljava/lang/String;J)V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), savePref, 
                j_key, j_name, value);

			pEnv->DeleteLocalRef(j_key);
			pEnv->DeleteLocalRef(j_name);
        }

        void PackageUtils::SavePreferenceBool(const acp_utils::helpers::SharedPreferenceContainer& spc, bool value)
        {
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			jstring j_key = pEnv->NewStringUTF(spc.key.c_str());
			jstring j_name = pEnv->NewStringUTF(spc.pName.c_str());

            jmethodID savePref = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "SavePreferenceBool", "(Ljava/lang/String;Ljava/lang/String;Z)V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), savePref, 
                j_key, j_name, value);

			pEnv->DeleteLocalRef(j_key);
			pEnv->DeleteLocalRef(j_name);
        }

        void PackageUtils::SavePreferenceString(const acp_utils::helpers::SharedPreferenceContainer& spc, const char* value)
        {
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			jstring j_key = pEnv->NewStringUTF(spc.key.c_str());
			jstring j_name = pEnv->NewStringUTF(spc.pName.c_str());
			jstring j_value = pEnv->NewStringUTF(value);

            jmethodID getPref = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "SavePreferenceString", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), getPref, 
                j_key, j_name, j_value);

			pEnv->DeleteLocalRef(j_key);
			pEnv->DeleteLocalRef(j_name);
			pEnv->DeleteLocalRef(j_value);
        }

        int PackageUtils::ReadSharedPreferenceInt(const acp_utils::helpers::SharedPreferenceContainer& spc, int defValue)
        {
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			jstring j_key = pEnv->NewStringUTF(spc.key.c_str());
			jstring j_name = pEnv->NewStringUTF(spc.pName.c_str());

            jmethodID getPref = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetPreferenceInt", "(Ljava/lang/String;Ljava/lang/String;I)I");
			int result = pEnv->CallStaticIntMethod(GetClass("/PackageUtils/AndroidUtils"), getPref, 
                j_key, j_name, defValue);

			pEnv->DeleteLocalRef(j_key);
			pEnv->DeleteLocalRef(j_name);
			
			return result;
        }

        long long PackageUtils::ReadSharedPreferenceLong(const acp_utils::helpers::SharedPreferenceContainer& spc, long defValue)
        {
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			jstring j_key = pEnv->NewStringUTF(spc.key.c_str());
			jstring j_name = pEnv->NewStringUTF(spc.pName.c_str());

            jmethodID getPref = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetPreferenceLong", "(Ljava/lang/String;Ljava/lang/String;J)J");
			long long result = pEnv->CallStaticLongMethod(GetClass("/PackageUtils/AndroidUtils"), getPref, 
                j_key, j_name, defValue);

			pEnv->DeleteLocalRef(j_key);
			pEnv->DeleteLocalRef(j_name);

			return result;
        }

        bool PackageUtils::ReadSharedPreferenceBool(const acp_utils::helpers::SharedPreferenceContainer& spc, bool defValue)
        {
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			jstring j_key = pEnv->NewStringUTF(spc.key.c_str());
			jstring j_name = pEnv->NewStringUTF(spc.pName.c_str());

            jmethodID getPref = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetPreferenceBool", "(Ljava/lang/String;Ljava/lang/String;Z)Z");
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), getPref, 
                j_key, j_name, defValue);

			pEnv->DeleteLocalRef(j_key);
			pEnv->DeleteLocalRef(j_name);

			return result;
        }

        std::string PackageUtils::ReadSharedPreferenceString(const acp_utils::helpers::SharedPreferenceContainer& spc, const std::string& defValue)
        {
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			jstring j_key = pEnv->NewStringUTF(spc.key.c_str());
			jstring j_name = pEnv->NewStringUTF(spc.pName.c_str());
			jstring j_defValue = (pEnv)->NewStringUTF(defValue.c_str());

            jmethodID getPref = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetPreferenceString", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");			
			jstring jResult = (jstring)pEnv->CallStaticObjectMethod(GetClass("/PackageUtils/AndroidUtils"), getPref, 
                j_key, j_name, j_defValue);


			pEnv->DeleteLocalRef(j_key);
			pEnv->DeleteLocalRef(j_name);
			pEnv->DeleteLocalRef(j_defValue);

			acp_utils::ScopeStringChars	res(pEnv, jResult);
			return res.Get();
        }

		void PackageUtils::RemoveSharedPreference(const acp_utils::helpers::SharedPreferenceContainer& spc)
		{
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			jstring j_key = pEnv->NewStringUTF(spc.key.c_str());
			jstring j_name = pEnv->NewStringUTF(spc.pName.c_str());

			jmethodID getPref = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "RemovePreference", "(Ljava/lang/String;Ljava/lang/String;)V");
			pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), getPref,
				j_key, j_name);


			pEnv->DeleteLocalRef(j_key);
			pEnv->DeleteLocalRef(j_name);
		}

        jobject PackageUtils::ReadSharedPreference(jobject& bundle)
        {
            JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);                     
            
            jmethodID getSharedPrefFunc = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "getPreference", "(Landroid/os/Bundle;)Landroid/os/Bundle;");
            return (jobject) pEnv->CallStaticObjectMethod(GetClass("/PackageUtils/AndroidUtils"), getSharedPrefFunc, bundle);
        }
        // End of shared preferences API

		// Show Welcome Screen
        void PackageUtils::ShowWelcomeScreen(acp_utils::helpers::Language language)
        {
			// deprecated, code ported to InGameBrowser library
        }


		void PackageUtils::ShowCannotGoBack()
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

            jmethodID showCannotGoBackID = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "ShowCannotGoBack", "()V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), showCannotGoBackID);
		}
		
		void PackageUtils::ShowToastMessage(const char* key, int duration)
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

			jstring jkey = pEnv->NewStringUTF(key);
				
            jmethodID showToastMessageID = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "ShowToastMessage", "(Ljava/lang/String;I)V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), showToastMessageID, jkey, duration);
			
			pEnv->DeleteLocalRef(jkey);
			
		}


		//Xamarin Include Line
		bool PackageUtils::IsUserMusicPlaying()
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jmethodID isUserMusicPlayingID = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "IsUserMusicPlaying", "()Z");
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), isUserMusicPlayingID);			

			return result;
		}

		const int&		PackageUtils::GetDensityDpi()
		{
			return s_DisplayInfo.nDensityDpi;
		}
		//Xamaring Include end
		
		//Logo Methods
		void PackageUtils::ShowLogo(const int& resId)
		{
			LOG_DBG("Showing Logo with res id %d", resId);

            JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

            jmethodID showLogo = pEnv->GetStaticMethodID(GetClass("/PackageUtils/LogoViewPlugin"), "ShowLogo", "(III)V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/LogoViewPlugin"), showLogo, resId, GetWidth(), GetHeight());
		}

		void PackageUtils::CloseLogo()
		{
			LOG_DBG("Hiding Logo");

            JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

            jmethodID closeLogo = pEnv->GetStaticMethodID(GetClass("/PackageUtils/LogoViewPlugin"), "CloseLogo", "()V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/LogoViewPlugin"), closeLogo);
		}
		
        // Browser Launch
        bool PackageUtils::LaunchBrowser(const char* url)
        {
            JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

            jstring       urlString = pEnv->NewStringUTF(url);
            
			jmethodID launchBrowser = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "LaunchBrowser", "(Ljava/lang/String;)Z");
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), launchBrowser, urlString);
			
			pEnv->DeleteLocalRef(urlString);

            return result;
		}


		//Http Execute Asynchronously 
		void PackageUtils::HttpExecuteAsync(const char* url)
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jstring  urlString = pEnv->NewStringUTF(url);

			jmethodID UrlResponse = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "HttpExecuteAsync", "(Ljava/lang/String;)V");

			pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), UrlResponse, urlString);

			pEnv->DeleteLocalRef(urlString);
		}
		

		////Http Synchronous Response 
		//std::string PackageUtils::GetHttpResponseURL(const char* url)
		//{
		//	JNIEnv* pEnv = NULL;
		//	ScopeGetEnv sta(pEnv);

		//	jstring  urlString = pEnv->NewStringUTF(url);
		//          
		//	jmethodID UrlResponse = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetHttpResponseURL", "(Ljava/lang/String;)Ljava/lang/String;");

		//	ScopeStringChars response_str(pEnv, (jstring)pEnv->CallStaticObjectMethod(GetClass("/PackageUtils/AndroidUtils"), UrlResponse, urlString));
		//	
		//	pEnv->DeleteLocalRef(urlString);

		//	return response_str.Get();
		//}

        // Launch Video Player
        bool PackageUtils::LaunchVideoPlayer(const char* filename)
        {
            JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

            // TODO
            jstring fNameString = pEnv->NewStringUTF(filename);

            jmethodID playVideo = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "LaunchVideoPlayer", "(Ljava/lang/String;)Z");
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), playVideo, fNameString);            
			
			pEnv->DeleteLocalRef(fNameString);

            return result;
        }

		bool PackageUtils::IsVideoCompleted()
        {
            JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

            jmethodID isVideoCompleted = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "IsVideoCompleted", "()I");
			int result = pEnv->CallStaticIntMethod(GetClass("/PackageUtils/AndroidUtils"), isVideoCompleted);            
			
			return (result != 0);
        }

        // Game specific
        void PackageUtils::SetGameSpecificIdentifiers(const acp_utils::helpers::GameSpecificIdentifiers& i_gsi)
        {
            s_GameSpecificIdentif = i_gsi;
        }

		void PackageUtils::SetDisplayInfo(const helpers::DisplayInfo& di)
		{
			s_DisplayInfo = di;
		}

        const std::string& PackageUtils::GetDefaultIGP()
        {
            return s_GameSpecificIdentif.sDefaultIgp;
        }

        const std::string& PackageUtils::GetGameName()
        {
            return s_GameSpecificIdentif.sGameName;
        }

        const std::string& PackageUtils::GetInjectedIGP()
        {
            return s_GameSpecificIdentif.sInjectedIgp;
        }

        const std::string& PackageUtils::GetInjectedSerialKey()
        {
            return s_GameSpecificIdentif.sInjectedSerialKey;
        }

		// Hardware Identifiers:
		void PackageUtils::SetHardwareIdentifiers(const acp_utils::helpers::HardwareIdentifiers& i_hwId)
		{
			LOG_DBG("Set hardware identifiers");
			s_HwIdentif = i_hwId;
		}

        const std::string& PackageUtils::GetAndroidId()
        {
            return s_HwIdentif.sAndroidId;
        }

        const std::string& PackageUtils::GetSerial()
        {
            return s_HwIdentif.sSerial;
        }

        const std::string& PackageUtils::GetCPUSerial()
        {
            return s_HwIdentif.sCpuSerial;
        }

        const std::string& PackageUtils::GetDeviceManufacturer()
        {
            return s_HwIdentif.sDeviceManufacturer;
        }

        const std::string& PackageUtils::GetCPUAbi(unsigned int index)
        {
			if (index < 0 || index >= s_HwIdentif.sCpuAbis.size())
				return "";
			return s_HwIdentif.sCpuAbis[index];
        }

		const std::vector<std::string>& PackageUtils::GetCPUAbis()
		{
			return s_HwIdentif.sCpuAbis;
		}

		const int PackageUtils::GetGpuMaxFreq()
		{
			std::vector<int> result;
			int resTmp;
			int finalResult;
			std::vector<std::string> sysPath;
			sysPath.push_back("/sys/devices/platform/kgsl-2d0.0/kgsl/kgsl-2d0/max_gpuclk");	//adreno
			sysPath.push_back("/sys/devices/platform/kgsl-3d0.0/kgsl/kgsl-3d0/max_gpuclk"); //adreno
			sysPath.push_back("/sys/class/kgsl/kgsl-3d0/max_gpuclk"); 						//adreno
			sysPath.push_back("/sys/devices/platform/omap/pvrsrvkm.0/sgxfreq/frequency_limit");
			sysPath.push_back("/sys/kernel/tegra_gpu/gpu_cap_rate");						//tegra
			sysPath.push_back("/sys/devices/platform/dfrgx/devfreq/dfrgx/max_freq");	 	//samsung galaxy tab 3 - PowerVR SGX540
			sysPath.push_back("/sys/devices/platform/gpusysfs/gpu_max_clock");		     	//samsung mali
			sysPath.push_back("/sys/kernel/gpu/gpu_max_clock");					 		 	//samsung adreno
			sysPath.push_back("/sys/class/devfreq/gpufreq/max_freq");

			for(std::string path : sysPath)
			{
				std::ifstream fGpu(path.c_str());
				std::string line;

				if(fGpu.is_open())
				{
					if(fGpu.good())
					{
						getline(fGpu, line);
					}
					fGpu.close();

					std::stringstream res(line);
					res >> resTmp;
					if(resTmp && resTmp > 0)
						result.push_back(resTmp);
				}
			}
			if(result.size() > 1)
			{
				//if we have more than one reported gpu freq value, we get the highest one.
				finalResult = result[0];
				for(int gpuFreq : result)
				{
					if(finalResult < gpuFreq)
						finalResult = gpuFreq;
				}
				//check to see if the reported unit is correct; some devices report in Hz
				//magic number comes from highest reported gpu frequencies found until now on tegra gpus
				if(finalResult > 1500)
				{
					//the unit is Hz -> we need MHz
					finalResult /= 1000000;
				}
				return finalResult;
			}
			if(result.size() == 1)
				return result[0];
			else return 0;
		}

		const int PackageUtils::GetNoOfAppearances(std::vector<int> vector, int value)
        {
            int noAppearances = 0;
            for(int i = 0; i < vector.size(); i++)
            {
                if(value == vector[i])
                    noAppearances++;
            }
            return noAppearances;
        }

		const int PackageUtils::GetNoOfAppearances(std::vector<std::string> vector, std::string value)
        {
            int noAppearances = 0;
            for(int i = 0; i < vector.size(); i++)
            {
                if(value == vector[i])
                    noAppearances++;
            }
            return noAppearances;
        }

		const std::string PackageUtils::GetCpuReport()
		{
			std::string result = "";
            int procNameSize;
			int noCpuCores;
			std::vector<int> freqChart = GetFreqChart();
            std::vector<std::string> procsNames = GetProcNames();
            if(procsNames.size() == GetNoOfAppearances(procsNames, procsNames[0]))
            {
                procNameSize = 1;
            }

			//name of the proc - no of cores, max freq
			std::map<const char*, std::pair<int, int>> procsMap;
			std::stringstream res;
            res.clear();
            //check how many processors it found; if only one was found, it means that it may wrongly report only a processor instead of multiple ones
            //so we treat this case differently
            bool isFirstTimeAdded = true;
            int freq = freqChart[0];
	    
	    //temporary fix for x86 crash on startup
	    if(freqChart.size() == 1 && freq == 0)
	    {
	    	return "N/A";
	    }
	    
	    if(procNameSize == 1)
            {
                int totalNoCores = 0;
                for(int j = 0; j < freqChart.size(); j++)
                {
                    if(freq == freqChart[j] && freq != 0)
                    {
                        if (isFirstTimeAdded)
                        {
                            noCpuCores = GetNoOfAppearances(freqChart, freqChart[j]);
                            totalNoCores += noCpuCores;
                            res << noCpuCores << "x " << procsNames[0] << "(" << freq << "Mhz); ";
                            isFirstTimeAdded = false;
                        }
                    }
                    else
                    {
                        freq = freqChart[j];
                        isFirstTimeAdded = true;
                        j--;
                    }
                    if(totalNoCores == GetNumberOfCpuCores())
                        j = freqChart.size();
                }
                return res.str();
            }
            std::string proc = procsNames[0];
            isFirstTimeAdded = true;
            int totalNoCores = 0;
			for(int i = 0; i < procsNames.size(); i++)
			{
                if(proc == procsNames[i])
                {
                    if(isFirstTimeAdded)
                    {
                        res << GetNoOfAppearances(procsNames, procsNames[i]) << "x " << procsNames[i] << "(" << freqChart[i] << "Mhz); ";
                        totalNoCores += GetNoOfAppearances(procsNames, procsNames[i]);
                        isFirstTimeAdded = false;
                    }
                }
                else
                {
                    proc = procsNames[i];
                    isFirstTimeAdded = true;
                    i--;
                }
                if(totalNoCores == GetNumberOfCpuCores())
                    i = procsNames.size();
			}
			return res.str();
		}

		const std::vector<int> PackageUtils::GetFreqChart()
		{
			std::vector<std::string> sysPath;
			std::vector<std::string> sysFile;
			std::string filePath = "";

			std::vector<int> freqChart;
			int cpuFreq;
			int noCpuCores = GetNumberOfCpuCores();

			sysPath.push_back("/sys/devices/system/cpu");
			sysPath.push_back("/sys/kernel/cpufreq_hardlimit");
			sysPath.push_back("/sys/kernel/msm_cpufreq_limit");

			sysFile.push_back("cpuinfo_max_freq");
			sysFile.push_back("scaling_max_freq");
			sysFile.push_back("scaling_max_freq_kt");
			sysFile.push_back("cpufreq_limit");

			for(int i=0; i<sysPath.size(); i++)
			{
				for(int j=0; j<sysFile.size(); j++)
				{
					if(i == 0)
					{
						filePath = sysPath[i] + "/cpu0/cpufreq/" + sysFile[j];
					}
					std::ifstream cpu(filePath.c_str());
					std::string line;

					if(cpu.is_open())
					{
						if(cpu.good())
						{
							getline(cpu, line);
						}
						cpu.close();

						std::stringstream res(line);
						res >> cpuFreq;
						if(cpuFreq && cpuFreq > 0)
						{
							freqChart.push_back(cpuFreq/1000);
						}
						else
						{
							freqChart.push_back(0);
						}

						//iterate through the rest of the cores to find their no. and frequencies
						for(int k=1; k<=noCpuCores; k++)
						{
							std::stringstream intToString;
							intToString << k;
							if(i == 0)
							{
								filePath = sysPath[i] + "/cpu" + intToString.str() + "/cpufreq/" + sysFile[j];
							}
							else
							{
								filePath = sysPath[i] + sysFile[j];
							}
							std::ifstream cpu_(filePath.c_str());
							if(cpu_.is_open())
							{
								if(cpu_.good())
								{
									getline(cpu_, line);
									std::stringstream res(line);
									res >> cpuFreq;
									if (cpuFreq && cpuFreq > 0)
										freqChart.push_back(cpuFreq/1000);	//transform to Mhz
									else freqChart.push_back(0);
								}
							}
							cpu_.close();
						}
					}
                    if(freqChart.size() == GetNumberOfCpuCores())
                        return freqChart;
				}
			}

            //temporary fix for x86 crash on startup
            if (freqChart.empty())
            {
                freqChart.push_back(0);
            }

            return freqChart;
		}

        const std::vector<std::string> PackageUtils::GetProcNames()
		{
			std::map <std::string, std::string> cpuParts;
			std::vector <std::string> cpuNames;

			cpuParts.insert(std::pair<std::string, std::string>("0xc05", "Cortex-A5"));
			cpuParts.insert(std::pair<std::string, std::string>("0xc07", "Cortex-A7"));
			cpuParts.insert(std::pair<std::string, std::string>("0xc08", "Cortex-A8"));
			cpuParts.insert(std::pair<std::string, std::string>("0xc09", "Cortex-A9"));

			cpuParts.insert(std::pair<std::string, std::string>("0xc0d", "Cortex-A12"));
			cpuParts.insert(std::pair<std::string, std::string>("0xc0e", "Cortex-A17"));
			cpuParts.insert(std::pair<std::string, std::string>("0xc0f", "Cortex-A15"));

			cpuParts.insert(std::pair<std::string, std::string>("0xd01", "Cortex-A32"));
			cpuParts.insert(std::pair<std::string, std::string>("0xd03", "Cortex-A53"));
			cpuParts.insert(std::pair<std::string, std::string>("0xd04", "Cortex-A35"));
			cpuParts.insert(std::pair<std::string, std::string>("0xd07", "Cortex-A57"));
			cpuParts.insert(std::pair<std::string, std::string>("0xd08", "Cortex-A72"));
			cpuParts.insert(std::pair<std::string, std::string>("0xd09", "Cortex-A73"));

			cpuParts.insert(std::pair<std::string, std::string>("0x0", "Denver"));
			cpuParts.insert(std::pair<std::string, std::string>("0x00", "Denver"));
			cpuParts.insert(std::pair<std::string, std::string>("0x000", "Denver"));

			cpuParts.insert(std::pair<std::string, std::string>("0xf", "Scorpion"));
			cpuParts.insert(std::pair<std::string, std::string>("0x2d", "Scorpion"));
			cpuParts.insert(std::pair<std::string, std::string>("0x4d", "Krait"));
			cpuParts.insert(std::pair<std::string, std::string>("0x6f", "Krait"));
			cpuParts.insert(std::pair<std::string, std::string>("0x06f", "Krait"));

			cpuParts.insert(std::pair<std::string, std::string>("0x201", "Kryo"));
			cpuParts.insert(std::pair<std::string, std::string>("0x205", "Kryo"));
			cpuParts.insert(std::pair<std::string, std::string>("0x211", "Kryo"));

			cpuParts.insert(std::pair<std::string, std::string>("0x800", "Kryo-2xx"));
			cpuParts.insert(std::pair<std::string, std::string>("0x801", "Kryo-2xx"));

			cpuParts.insert(std::pair<std::string, std::string>("0x1", "Mongoose"));
			cpuParts.insert(std::pair<std::string, std::string>("0x01", "Mongoose"));
			cpuParts.insert(std::pair<std::string, std::string>("0x001", "Mongoose"));


			std::ifstream fCpu("/proc/cpuinfo");
			const char* property = "CPU part";
			const char* splitter = ":";
			std::string line;
			if(fCpu.is_open())
			{
				while(fCpu.good() && !fCpu.eof())
				{
					getline (fCpu, line);
					if (strcmp(property, ""))
					{
						if(line.find(property) != 0)
						{
							continue;
						}

						line = line.substr(strlen(property), std::string::npos);
						line = line.substr(line.find(splitter) + 1, std::string::npos);

						line.erase(line.begin(), std::find_if(line.begin(), line.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));

						std::string result;
						for(std::string::iterator tempItr = line.begin(); tempItr != line.end(); tempItr++)
						{
							if(!std::iscntrl((int)(unsigned char)*tempItr))
							{
								result.push_back(*tempItr);
							}
						}

						cpuNames.push_back(result);
					}
				}
			}
			fCpu.close();

			for(std::map<std::string, std::string> ::iterator it = cpuParts.begin(); it != cpuParts.end(); it++)
			{
                for(int i=0; i<cpuNames.size(); i++)
                {
                    if(it->first.compare(cpuNames[i]) == 0)
                    {
                        cpuNames[i] = it->second;
                    }
                }
			}

            //temporary fix for x86 crash
            if (cpuNames.empty())
            {
                cpuNames.push_back("non-arm");
            }

            return cpuNames;
		}

        const std::string& PackageUtils::GetDeviceModel()
        {
            return s_HwIdentif.sDeviceModel;
        }
		
        const std::string& PackageUtils::GetBuildProduct()
        {
            return s_HwIdentif.sBuildProduct;
        }

        const std::string& PackageUtils::GetBuildDevice()
        {
            return s_HwIdentif.sBuildDevice;
        }

        const std::string& PackageUtils::GetFirmware()
        {
            return s_HwIdentif.sFirmware;
        }

        const std::string& PackageUtils::GetMacAddress()
        {
            // TODO -- Do we want the mac address or the ethernet address?
            // Does an ethernet address even exist? Is this for TVs having ethernet ?
            // Do TV's having ethernet also have wifi ?
            return s_HwIdentif.sWiFiMacAddress;
        }

		const std::string& PackageUtils::GetIMEI()
		{
			return s_HwIdentif.sImei;
		}

        const std::string& PackageUtils::GetHDIDFVStr()
        {
            return s_HwIdentif.sHdidfv;
        }

		const float	PackageUtils::GetWidthInInch()
		{
			return s_DisplayInfo.nWidth / s_DisplayInfo.xDpi;
		}
		
		const float	PackageUtils::GetHeightInInch()
		{
			return s_DisplayInfo.nHeight / s_DisplayInfo.yDpi;
		}

		const float PackageUtils::GetDisplaySizeInInch()
		{
		    float width = GetWidthInInch();
		    float height = GetHeightInInch();

		    float squared = width * width + height * height;
		    return sqrt(squared);
		}

		const int&		PackageUtils::GetWidth()
		{
			return s_DisplayInfo.nWidth;
		}
		
		const int&		PackageUtils::GetHeight()
		{
			return s_DisplayInfo.nHeight;
		}

		// Software identifiers
		void PackageUtils::SetSoftwareIdentifiers(const acp_utils::helpers::SoftwareIdentifiers& i_swId)
		{
			s_SwIdentif = i_swId;
		}
		
		// User Location
		void PackageUtils::SetUserLocation(const acp_utils::helpers::UserLocation& i_usrLocation)
		{
			s_UserLocation = i_usrLocation;
		}

        const std::string& PackageUtils::GetCarrierName()
        {
            return s_SwIdentif.sCarrierName;
        }

        const std::string& PackageUtils::GetCountry()
        {
            return s_SwIdentif.sCountry;
        }

        const std::string& PackageUtils::GetDeviceLanguage()
        {
            return s_SwIdentif.sDeviceLanguage;
        }        
		
		const std::string& PackageUtils::GetWebviewUserAgent()
		{
			return s_SwIdentif.sWebviewUserAgent;
		}

		const std::string& PackageUtils::GetApkPath()
		{
			return s_SwIdentif.sApkPath;
		}
		
		// GoogleAdId
		const std::string& PackageUtils::GetGoogleAdId()
        {
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			static std::string google_adid;
			// Google Ad Id              
            jmethodID getGoogleAdId = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetGoogleAdId", "()Ljava/lang/String;");
			ScopeStringChars gaidString(pEnv, (jstring)pEnv->CallStaticObjectMethod(GetClass("/PackageUtils/AndroidUtils"), getGoogleAdId));
			google_adid = gaidString.Get();

			return google_adid;
        }
		
		const acp_utils::helpers::GAIDStatus PackageUtils::GetGoogleAdIdStatus()
        {
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			static acp_utils::helpers::GAIDStatus	aid_status;
			// Google Ad Id Status
            jmethodID getGoogleAdIdStatus = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetGoogleAdIdStatus", "()I");
			aid_status = (acp_utils::helpers::GAIDStatus) pEnv->CallStaticIntMethod(GetClass("/PackageUtils/AndroidUtils"), getGoogleAdIdStatus);

            return aid_status;
        }

		bool PackageUtils::IsMusicActive()
		{
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);
         
			jmethodID musicActive = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "IsMusicActive", "()Z");
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), musicActive);

			return result;
		}
		
		bool PackageUtils::IsSimReady()
		{
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);
         
			jmethodID simReady = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "IsSimReady", "()Z");
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), simReady);

			return result;
		}

		// BatteryInfo:
        void PackageUtils::SetBatteryInfo(const acp_utils::helpers::BatteryInfo& i_batteryInfo)
        {
            LOG_DBG("Set Battery Info");
            s_BatteryInfo = i_batteryInfo;
        }

        const bool PackageUtils::GetIsBatteryCharging()
        {
            return s_BatteryInfo.sIsCharging;
        }

        const bool PackageUtils::GetIsUsbCharging()
        {
            return s_BatteryInfo.sUsbCharge;
        }

        const bool PackageUtils::GetIsACCharging()
        {
            return s_BatteryInfo.sACCharge;
        }

        const int PackageUtils::GetBatteryStatus()
        {
            return s_BatteryInfo.sBatteryStatus;
        }

        const int PackageUtils::GetLastMemoryWarning()
        {			
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);
         
			jmethodID warningFunction = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "IsMemoryWarningActive", "()I");
			int level = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), warningFunction);

			return level;
        }

		void PackageUtils::SetOrientationState(bool i_bUnlock)
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

            jmethodID orientation_func = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "SetOrientation", "(Z)V");
            return pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), orientation_func, i_bUnlock);
		}
		
		void PackageUtils::SetKeepScreenOn(bool i_bKeepScreen)
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

            jmethodID orientation_func = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "SetKeepScreenOn", "(Z)V");
            return pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), orientation_func, i_bKeepScreen);
		}

		std::string	PackageUtils::ReadInfoFromSystemFile(const char* path, const char* property, const char* splitter)
		{
			std::string line;
			std::ifstream myfile (path);
			if (myfile.is_open())
			{
				while ( myfile.good() )
				{
					getline (myfile, line);
					
					if (strcmp(property, ""))
					{
						if (line.find(property) != 0)
						{
							continue;
						}

						line = line.substr(strlen(property), std::string::npos);
						line = line.substr(line.find(splitter) + 1, std::string::npos);

						line.erase(line.begin(), std::find_if(line.begin(), line.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
						
						std::string result;
						for (std::string::iterator tempItr = line.begin(); tempItr != line.end(); tempItr++)
						{
							if (!std::iscntrl((int)(unsigned char)*tempItr))
							{
								result.push_back(*tempItr);
							}
						}
						return result;
					}
					else if (line.compare(""))
					{
						break;
					}
				}
				myfile.close();
			}
			else 
			{
				LOG_ERROR("CDeviceSpecs::readInfoFromFile fail (%s, %s): %s", property , splitter, path);
			}
			return line;
		}
		
		unsigned long long	PackageUtils::GetDiskFreeSpace()
		{
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

            jmethodID getDiskSpace = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetDiskFreeSpace", "(Ljava/lang/String;)J");

			jstring j_Path = (pEnv)->NewStringUTF(GetDataFolderPath().c_str());

			unsigned long long result = pEnv->CallStaticLongMethod(GetClass("/PackageUtils/AndroidUtils"), getDiskSpace, j_Path);

			pEnv->DeleteLocalRef(j_Path);

			return result;
		}

		unsigned int PackageUtils::GetDiskTotalSpace()
		{
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			jmethodID getTotalSpace = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetDiskTotalSpace", "()I");

			int result = pEnv->CallStaticIntMethod(GetClass("/PackageUtils/AndroidUtils"), getTotalSpace);

			return result;
		}
		
		unsigned long long	PackageUtils::GetTaskMemory()
		{
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

            jmethodID getTaskMemory = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetTaskMemory", "()J");

			unsigned long long result = pEnv->CallStaticLongMethod(GetClass("/PackageUtils/AndroidUtils"), getTaskMemory);

			return result;
		}
		//Xamarin Include End

		int		PackageUtils::GetNumberOfCpuCores()
		{
			static int numberOfCores = -1;

			if(numberOfCores == -1)
			{
				const char* path = "/sys/devices/system/cpu/";

				struct dirent *dp;
				DIR *fd;
				int result = 0;

				if ((fd = opendir(path)) == NULL)
				{
					LOG_ERROR("numberOfFiles: can't open %s", path);
					return 0;
				}
				while ((dp = readdir(fd)) != NULL)
				{
					if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") || (strlen(dp->d_name) != 4) || (strstr(dp->d_name, "cpu") != dp->d_name))
						continue;
					result++;
				}
				closedir(fd);
				numberOfCores = result;

			}
			//LOG_DBG("number of cores = %d", numberOfCores);
			return numberOfCores;
		}

		int	PackageUtils::GetCurrentCpuSpeedInHz(int cpu)
		{
			char buffer[64];
			
			sprintf(buffer, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", cpu);
			std::string cpuSpeedStr = ReadInfoFromSystemFile(buffer, "", "");
			
			float cpuSpeedCurrent = atoi(cpuSpeedStr.c_str());
			return cpuSpeedCurrent;
		}

		int	PackageUtils::GetMaxCpuSpeedInHz()
		{
			static int nMaxCpuSpeed = -1;
			if(nMaxCpuSpeed < 0)
			{
				std::string cpu = ReadInfoFromSystemFile("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "", "");
				nMaxCpuSpeed = atoi(cpu.c_str());
			}

			return nMaxCpuSpeed;
		}

		std::string PackageUtils::GetDeviceChipset(void)
		{
			static std::string chipsetTemp = ReadInfoFromSystemFile("/system/build.prop", "ro.chipname", "=");
            //some devices don't have the "ro.chipname" field, so we're looking at another one similar to it
            if(chipsetTemp.empty())
                chipsetTemp = ReadInfoFromSystemFile("/system/build.prop", "ro.board.platform", "=");
            //make all the characters lowercase so we have the same results regardless of the source
            std::transform(chipsetTemp.begin(), chipsetTemp.end(), chipsetTemp.begin(), ::tolower);
			static std::string chipset;

			// Remove control characters
			chipset.clear();
			for(std::string::iterator tempItr = chipsetTemp.begin(); tempItr != chipsetTemp.end(); tempItr++)
			{
				if (!std::iscntrl((int)(unsigned char)*tempItr))
				{
					chipset.push_back(*tempItr);
				}
			}
			
			LOG_DBG("GetDeviceChipset %s", chipset.c_str());

			return chipset;
		}

		std::string PackageUtils::GetDeviceArchitecture(void)
		{
			static std::string arch = ReadInfoFromSystemFile("/proc/cpuinfo", "CPU implementer", ":");

			LOG_DBG("GetDeviceArchitecture %s", arch.c_str());

			return arch;
		}

		std::string PackageUtils::GetDeviceMicroArch(void)
		{
			static std::string microArch = ReadInfoFromSystemFile("/proc/cpuinfo", "CPU part", ":");

			LOG_DBG("GetDeviceMicroArch %s", microArch.c_str());

			return microArch;
		}

		float		PackageUtils::GetCurrentAvailableRamInMegaBytes()
		{
			std::string mem = ReadInfoFromSystemFile("/proc/meminfo", "MemFree", ":");
			float result = atoi(mem.c_str());
			if(mem.find("kB") != std::string::npos)
			{
				result = result / 1024.0f;
			}
			else if(mem.find("MB") != std::string::npos)
			{
				//result stays the same
			}
			else
			{
				LOG_DBG("Memory in meminfo is not in kB nor in MB. Returning the value itself...");
			}

			LOG_DBG("GetCurrentAvailableRamInMegaBytes = %s", mem.c_str());

			return result;
		}

		float	PackageUtils::GetMaxAvailableRamInMegaBytes()
		{
			static float s_nMaxMem = -1.0f;
			if(s_nMaxMem < 0.0f)
			{
				std::string mem = ReadInfoFromSystemFile("/proc/meminfo", "MemTotal", ":");
				float result = atoi(mem.c_str());
				if(mem.find("kB") != std::string::npos)
				{
					result = result / 1024.0f;
				}
				else if(mem.find("MB") != std::string::npos)
				{
					//result stays the same
				}
				else
				{
					LOG_DBG("Memory in meminfo is not in kB nor in MB. Returning the value itself...");
				}

				LOG_DBG("mem = %s", mem.c_str());

				s_nMaxMem = result;
			}

			LOG_DBG("GetMaxAvailableRamInMegaBytes = %f", s_nMaxMem);

			return s_nMaxMem;
		}

		bool	PackageUtils::IsDeviceRouted()
		{
			static int CheckRout = -1;
			if(CheckRout == -1)
			{
				bool rtdDevice (false);

				// #1st method
				FILE * pToSUApkFile (fopen ("/system/app/Superuser.apk", "rb"));
				if (NULL != pToSUApkFile) {
					// Erase & set
					fclose (pToSUApkFile);
					rtdDevice = true;
				}

				// #2nd method
				if (false == rtdDevice) {
					struct stat sb;
					if (stat ("/system/bin/su", &sb) == -1 && stat ("/system/xbin/su", &sb) == -1) {
						rtdDevice = false;
					} else {
						rtdDevice = true;
					}
				}
				CheckRout = static_cast<int>(rtdDevice);
			}

			return static_cast<bool>(CheckRout);
		}

		bool	PackageUtils::IsAppEnc()
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

            jmethodID isAppEnc = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "retrieveBarrels", "()[I");
			jintArray arr ((jintArray) pEnv->CallStaticObjectMethod (GetClass("/PackageUtils/AndroidUtils"), isAppEnc));

			// Check
			if (arr) {
				jsize len (pEnv->GetArrayLength (arr));
				jint * body (pEnv->GetIntArrayElements (arr, 0));
				// Check
				if (NULL == body) {
					return true;
				}
				// Check
				int value[2] = {16847, 2021}; // Target is 4042 50541
				bool ret (true); // True to avoid false positives
				// For
				for (int i = 0; i < (int) len; ++i) {
					// Check
					if (i == 0) {
						value[1] *= 2; // Get 4042
					}
					// Check
					if (body[i] == (value[1] * 100000 + (value[0] * 3))) { // == 404250541 GLSignature for Google Play
						pEnv->ReleaseIntArrayElements (arr, body, 0);
						pEnv->DeleteLocalRef(arr);
						return true;
					} else if (body[i] > 0) {
						// Set false if any of the value is incorrect
						ret = false;
					}
				}
				// Erase
				pEnv->ReleaseIntArrayElements (arr, body, 0);
				pEnv->DeleteLocalRef(arr);
				return ret;
			}
			// Avoid false positives
			return true;
		}

		bool PackageUtils::AreNotificationsEnabled()
		{
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);

			jmethodID notificationsEnabled = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "AreNotificationsEnabled", "()Z");
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), notificationsEnabled);
			return result;
		}

		const std::string PackageUtils::GetObbFilePaths(const char* delimiter)
		{
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);
			jstring strDelimiter = pEnv->NewStringUTF(delimiter);

			jmethodID getObbFilePaths = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetObbFilePaths", "(Ljava/lang/String;)Ljava/lang/String;");
			jstring filePaths = (jstring)pEnv->CallStaticObjectMethod(GetClass("/PackageUtils/AndroidUtils"), getObbFilePaths, strDelimiter);

			const char* temp = pEnv->GetStringUTFChars(filePaths, NULL);
			std::string value = temp;

			pEnv->ReleaseStringUTFChars(filePaths, temp);
			pEnv->DeleteLocalRef(strDelimiter);

			return value;
		}

//dark RENDER TARGET
		bool PackageUtils::CheckIfDeviceIsATablet()
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jmethodID checkIfDeviceIsATablet = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "checkIfDeviceIsATablet", "()Z");
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), checkIfDeviceIsATablet);
			return result;
		}

#if AMAZON_STORE
		bool PackageUtils::IsDeviceKindle()
		{
			std::string manuf = acp_utils::api::PackageUtils::GetDeviceManufacturer();
			std::transform(manuf.begin(), manuf.end(), manuf.begin(), ::tolower);

			if(manuf.find("ti") != std::string::npos || manuf.find("amazon") != std::string::npos)
			{
				return true;
			}

			return false;
		}

		void PackageUtils::HideKindleBar()
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

			jmethodID showBar = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "FullScreenToggleShowBar", "()V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), showBar);            
		}

		void PackageUtils::ShowKindleBar()
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

			jmethodID hideBar = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "FullScreenToggleHideBar", "()V");
            pEnv->CallStaticVoidMethod(GetClass("/PackageUtils/AndroidUtils"), hideBar);            
		}
#endif //AMAZON_STORE

		std::string PackageUtils::GenerateUUID()
		{
			JNIEnv* pEnv = NULL;
            ScopeGetEnv sta(pEnv);

			jmethodID generateUUID = pEnv->GetStaticMethodID(GetClass("/GLUtils/SUtils"), "GenerateUUID", "()Ljava/lang/String;");
            jstring uuidString = (jstring)pEnv->CallStaticObjectMethod(GetClass("/GLUtils/SUtils"), generateUUID);
			
			const char* uuid = pEnv->GetStringUTFChars(uuidString, NULL);
			std::string result = uuid;
			
			pEnv->ReleaseStringUTFChars(uuidString, uuid);
			return result;
		}
		
		// Browser Launch
		bool PackageUtils::LaunchWifiSettings()
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jmethodID launchWifiSettings = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "LaunchWifiSettings", "()Z");
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), launchWifiSettings);

			return result;
		}
		
		
		bool PackageUtils::ShowDialog(const std::string& title, const std::string& text, bool haveCancel)
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jstring jTitle = pEnv->NewStringUTF(title.c_str());
			jstring jText = pEnv->NewStringUTF(text.c_str());
			
			jmethodID showGameDialog = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "ShowGameDialog", "(Ljava/lang/String;Ljava/lang/String;Z)Z");
			bool result = pEnv->CallStaticBooleanMethod(GetClass("/PackageUtils/AndroidUtils"), showGameDialog, jTitle, jText, haveCancel);

			pEnv->DeleteLocalRef(jTitle);
			pEnv->DeleteLocalRef(jText);

			return result;
		}
#if USE_ANTI_HACK_FOR_PHD
		std::string	PackageUtils::GetPlainText(const char* input, const char* key)
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);
			jstring strKey = pEnv->NewStringUTF(key);
			jstring strInput = pEnv->NewStringUTF(input);

			jmethodID getPlainText = pEnv->GetStaticMethodID(GetClass("/GLUtils/SUtils"), "GetPlainText", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
			jstring plainText = (jstring)pEnv->CallStaticObjectMethod(GetClass("/GLUtils/SUtils"), getPlainText, strInput, strKey);
			
			const char* temp = pEnv->GetStringUTFChars(plainText, NULL);

			std::string value = temp;
			
			pEnv->ReleaseStringUTFChars(plainText, temp);
			pEnv->DeleteLocalRef(strKey);
			pEnv->DeleteLocalRef(strInput);
			pEnv->DeleteLocalRef(plainText); //khoai
			return value;
		}
		void PackageUtils::ShowOfflineZoroDialog()
		{
			JNIEnv* pEnv = NULL;
			ScopeGetEnv sta(pEnv);

			jmethodID ShowOfflineZoroDialog = pEnv->GetStaticMethodID(GetClass("/GLUtils/SUtils"), "ShowOfflineZoroDialog", "()V");
			pEnv->CallStaticVoidMethod(GetClass("/GLUtils/SUtils"), ShowOfflineZoroDialog);
		}
#endif

#if USE_CRASHLYTICS && 0
		bool PackageUtils::isCrashlyticsEnabled()
		{
			acp_utils::helpers::SharedPreferenceContainer spc;
			spc.key   = "CrashlyticsEnabled";
			spc.pName = "CrashlyticsPrefs";
			
			return ReadSharedPreferenceBool( spc , true );
		}

		void PackageUtils::setCrashlyticsEnabled(bool enabled)
		{
			acp_utils::helpers::SharedPreferenceContainer spc;
			spc.key   = "CrashlyticsEnabled";
			spc.pName = "CrashlyticsPrefs";
			SavePreferenceBool( spc , enabled );
		}
#endif

#if GEF_OBFUSCATE_USE_INSTALLER // Uyen: Added
		const std::string PackageUtils::GetAllDataPaths(const char* delimiter)
		{
			JNIEnv* pEnv;
			ScopeGetEnv st(pEnv);
			jstring strDelimiter = pEnv->NewStringUTF(delimiter);

			jmethodID getAllDataPaths = pEnv->GetStaticMethodID(GetClass("/PackageUtils/AndroidUtils"), "GetAllDataPaths", "(Ljava/lang/String;)Ljava/lang/String;");
			jstring filePaths = (jstring)pEnv->CallStaticObjectMethod(GetClass("/PackageUtils/AndroidUtils"), getAllDataPaths, strDelimiter);

			const char* temp = pEnv->GetStringUTFChars(filePaths, NULL);
			std::string value = temp;

			pEnv->ReleaseStringUTFChars(filePaths, temp);
			pEnv->DeleteLocalRef(strDelimiter);

			return value;
		}
#endif

	}//end namespace api



	//hack for switching the place for jvm
	JavaVM* GetVM()
	{
		return api::PackageUtils::GetJavaVm();
	}

};//end namespace acp_utils
