#include <jni.h>
#include <android/log.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#define LOG_TAG "QHEngine"
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern void Init_MainAndroid(ANativeWindow * window);
extern void Resize(int w, int h);
extern void Update(long deltaTime);
extern void CleanUp();
extern void OnGameTouchEvent(int eventId, int x, int y, int pointerId);

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_learnning3D_GLES3JNILib_InitMainAndroid(JNIEnv* env, jobject obj, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_android_learnning3D_GLES3JNILib_InitWithNativeWindow(JNIEnv* env, jobject obj, jobject surface);
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
Java_com_android_learnning3D_GLES3JNILib_InitWithNativeWindow(JNIEnv* env, jobject obj, jobject surface)
 {
	ANativeWindow *window = ANativeWindow_fromSurface(env, surface);

     //Init_With_NativeWindows(static_cast<ANativeWindow*>(window));
	 Init_MainAndroid(static_cast<ANativeWindow*>(window));
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