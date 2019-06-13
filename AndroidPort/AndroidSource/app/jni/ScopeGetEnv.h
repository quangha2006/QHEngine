#ifndef 	__SCOPE_GET_ENV_H__
#define		__SCOPE_GET_ENV_H__

#include <jni.h>



namespace acp_utils
{
	extern JavaVM* GetVM();

	class ScopeGetEnv
	{
		bool 		m_bThreadAttached;
		
	public:
	
		ScopeGetEnv(JNIEnv*& env)
		{
			m_bThreadAttached = false;
			switch (GetVM()->GetEnv((void**)&env, JNI_VERSION_1_6))
			{
				case JNI_OK:
					break;
				case JNI_EDETACHED:
					if (GetVM()->AttachCurrentThread(&env, 0)!=0)
					{
						//throw std::runtime_error("Could not attach current thread");
					}
					m_bThreadAttached = true;
					break;
				case JNI_EVERSION:
					//throw std::runtime_error("Invalid java version");
					break;
			}	
		};
		
		
		~ScopeGetEnv()
		{
			if(m_bThreadAttached)
			{
				(GetVM())->DetachCurrentThread();
			}
		};
	};

}

#endif //__SCOPE_GET_ENV_H__