#include <jni.h>
#include "GameFunctionsToImplement.h"
#include <android/log.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#define LOG_TAG "QHEngine"
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_learnning3D_GLES3JNILib_InitMainAndroid(JNIEnv* env, jobject obj, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_android_learnning3D_GLES3JNILib_InitWithNativeWindow(JNIEnv* env, jobject obj, jint width, jint height, jobject surface);
    JNIEXPORT void JNICALL Java_com_android_learnning3D_GLES3JNILib_Update(JNIEnv* env, jobject obj, jlong deltaltime);
    JNIEXPORT void JNICALL Java_com_android_learnning3D_GLES3JNILib_Resize(JNIEnv* env, jobject obj, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_learnning3D_GLES3JNILib_CleanUp(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_android_learnning3D_GLES3JNILib_OnGameTouchEvent(JNIEnv* env, jobject obj, jint eventId, jfloat x, jfloat y, jint pointerId);
};

JNIEXPORT void JNICALL
Java_com_android_learnning3D_GLES3JNILib_InitMainAndroid(JNIEnv* env, jobject obj, jint width, jint height)
{
      //Init_MainAndroid(width, height);
}
JNIEXPORT void JNICALL
Java_com_android_learnning3D_GLES3JNILib_InitWithNativeWindow(JNIEnv* env, jobject obj, jint width, jint height, jobject surface)
 {

    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
     LOG_INFO("Got window %p", window);
	
    //int width = ANativeWindow_getWidth(window);
 	//int height = ANativeWindow_getHeight(window);
	//ANativeWindow_setBuffersGeometry(window, width * 0.5f, height * 0.5f);
 	//LOG_INFO("Got window %d %d", width,height);

 	LOG_INFO("window buffer count %d",window);

     //Init_With_NativeWindows(static_cast<ANativeWindow*>(window));
	 Init_MainAndroid(width, height, static_cast<ANativeWindow*>(window));
 }
JNIEXPORT void JNICALL
Java_com_android_learnning3D_GLES3JNILib_Update(JNIEnv* env, jobject obj, jlong deltaltime)
{
    Update(deltaltime);
}
JNIEXPORT void JNICALL
Java_com_android_learnning3D_GLES3JNILib_Resize(JNIEnv* env, jobject obj, jint width, jint height)
{
	Resize(width, height);
}

JNIEXPORT void JNICALL
Java_com_android_learnning3D_GLES3JNILib_CleanUp(JNIEnv* env, jobject obj)
{
		CleanUp();
}
JNIEXPORT void JNICALL
Java_com_android_learnning3D_GLES3JNILib_OnGameTouchEvent(JNIEnv* env, jobject obj, jint eventId, jfloat x, jfloat y, jint pointerId)
{
		OnGameTouchEvent(eventId, x, y, pointerId);
}