package com.android.QHEngine;

import android.view.Surface;
import android.opengl.GLSurfaceView;

public class GLES3JNILib {
    static {
        System.loadLibrary("native-activity");
    }

    public static native void InitMainAndroid(int width, int height);
    public static native void InitWithNativeWindow(Surface surface);
    public static native void Resize(int width, int height);
    public static native void Update(long delta);
    public static native void OnGameTouchEvent(int eventId, float x, float y, int pointerId);
    public static native void CleanUp();
}
