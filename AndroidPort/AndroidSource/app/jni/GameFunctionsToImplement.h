//#ifndef __GAME_FUNC_TO_IMPLEMENT_H__
//#define __GAME_FUNC_TO_IMPLEMENT_H__

/*Require includes for extern functions*/

//#include <android/native_window_jni.h> //for ANativeWindow
//#include <android/native_window.h> // requires ndk r5 or newer

// All the following functions are callbacks.
// Please implement them in the game.
// If you don't require the functionality, you may implement them as NOPs.


//extern void Init();

extern void Init_MainAndroid(int w, int h);

extern void Resize(int w, int h);

extern void Update(long deltaTime);

extern void CleanUp();

extern void OnGameTouchEvent(int eventId, int x, int y, int pointerId);
