package com.android.learnning3D;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.view.Surface;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.util.DisplayMetrics;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class GLES3View extends GLSurfaceView{

    static int widthPixels = 0;
    static int heightPixels = 0;
    float scaleSurfaceView = 0.8f;
    static GLSurfaceView glSurfaceView = null;
    public GLES3View(Context context) {
    
        super(context);

        DisplayMetrics metrics = getResources().getDisplayMetrics();

        widthPixels = metrics.widthPixels;
        heightPixels = metrics.heightPixels;
        this.getHolder().setFixedSize((int)(widthPixels * scaleSurfaceView),(int)(heightPixels * scaleSurfaceView));
        setEGLConfigChooser(new MyConfigChooser());

        final ActivityManager activityManager = (ActivityManager) context.getSystemService(context.ACTIVITY_SERVICE);
        final ConfigurationInfo configurationInfo;
        configurationInfo = activityManager.getDeviceConfigurationInfo();

        final boolean supportsEs3 = configurationInfo.reqGlEsVersion >= 0x30000;
        if (supportsEs3) {
            setEGLContextClientVersion(3);
        }
        else {
            setEGLContextClientVersion(2);
        }
        glSurfaceView = this;
        setPreserveEGLContextOnPause(true);
        setRenderer(new Renderer());

    }
    public boolean onTouchEvent(MotionEvent event)
    {
        int pointerIndex = event.getActionIndex();

        // get pointer ID
        int pointerId = event.getPointerId(pointerIndex);

        // get masked (not specific to a pointer) action
        int maskedAction = event.getActionMasked();

        boolean retCode = false;

        switch (maskedAction)
        {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
            {
                GLES3JNILib.OnGameTouchEvent(0, event.getX(pointerIndex) * scaleSurfaceView, event.getY(pointerIndex) * scaleSurfaceView, pointerId);
                retCode = true;
                break;
            }
            case MotionEvent.ACTION_MOVE:
            {
                for (int size = event.getPointerCount(), i = 0; i < size; i++)
                {
                    GLES3JNILib.OnGameTouchEvent(1, event.getX(i) * scaleSurfaceView, event.getY(i) * scaleSurfaceView, event.getPointerId(i));
                }

                retCode = true;
                break;
            }
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
            {
                GLES3JNILib.OnGameTouchEvent(2, event.getX(pointerIndex) * scaleSurfaceView, event.getY(pointerIndex) * scaleSurfaceView, pointerId);
                retCode = true;
                break;
            }
            case MotionEvent.ACTION_CANCEL:
            {
                GLES3JNILib.OnGameTouchEvent(3, event.getX(pointerIndex) * scaleSurfaceView, event.getY(pointerIndex) * scaleSurfaceView, pointerId);
                retCode = true;
                break;
            }
            default:
            {
                retCode = false;
                break;
            }
        }
        return retCode;
    }
    public boolean onKeyDown(final int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK){
            //GLES3JNILib.NativeOnDestroy();
            //System.exit(1);
        }
        return false;
    }
    private static class Renderer implements GLSurfaceView.Renderer {
        public void onDrawFrame(GL10 gl) {
            GLES3JNILib.Update((long)0);
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            GLES3JNILib.Resize(width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            GLES3JNILib.InitWithNativeWindow(glSurfaceView.getHolder().getSurface());
        }
    }
}
class MyConfigChooser implements GLSurfaceView.EGLConfigChooser {
    @Override
    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
        int attribs[] = {
            EGL10.EGL_LEVEL, 0,
            EGL10.EGL_RENDERABLE_TYPE, 4,  // EGL_OPENGL_ES2_BIT
            EGL10.EGL_COLOR_BUFFER_TYPE, EGL10.EGL_RGB_BUFFER,
            EGL10.EGL_RED_SIZE, 8,
            EGL10.EGL_GREEN_SIZE, 8,
            EGL10.EGL_BLUE_SIZE, 8,
            EGL10.EGL_ALPHA_SIZE, 8,
            EGL10.EGL_DEPTH_SIZE, 24,
            EGL10.EGL_SAMPLE_BUFFERS, 1,
            EGL10.EGL_SAMPLES, 4,  // This is for 4x MSAA.
            EGL10.EGL_NONE
        };

        int[] numConfigs = new int[1];
        egl.eglChooseConfig(display, attribs, null, 0, numConfigs);
        EGLConfig[] configs = new EGLConfig[numConfigs[0]];
        egl.eglChooseConfig(display, attribs, configs, numConfigs[0], numConfigs);

        if (numConfigs[0] == 0) {
            Log.e("QHEngine","Failed to chose config!");
            return null;
        } else {
            if (false) // print configs
            {
                int[] r = new int[1];
                int[] g = new int[1];
                int[] b = new int[1];
                int[] a = new int[1];
                int[] d = new int[1];
                int[] s = new int[1];
                int[] sb = new int[1];
                int[] sa = new int[1];
                int i = 0;
                for (; i < numConfigs[0]; i++) {
                    EGLConfig cfg = configs[i];
                    egl.eglGetConfigAttrib(display, cfg, EGL10.EGL_RED_SIZE, r);
                    egl.eglGetConfigAttrib(display, cfg, EGL10.EGL_GREEN_SIZE, g);
                    egl.eglGetConfigAttrib(display, cfg, EGL10.EGL_BLUE_SIZE, b);
                    egl.eglGetConfigAttrib(display, cfg, EGL10.EGL_ALPHA_SIZE, a);
                    egl.eglGetConfigAttrib(display, cfg, EGL10.EGL_DEPTH_SIZE, d);
                    egl.eglGetConfigAttrib(display, cfg, EGL10.EGL_STENCIL_SIZE, s);
                    egl.eglGetConfigAttrib(display, cfg, EGL10.EGL_SAMPLE_BUFFERS, sb);
                    egl.eglGetConfigAttrib(display, cfg, EGL10.EGL_SAMPLES, sa);

                    Log.i("QHEngine", "R:"+ r[0] + " G:"+ g[0] + " B:"+b[0] + " A:"+a[0] + " D:"+d[0] + " S:"+s[0] + " SB:"+ sb[0]+ " SA:" +sa[0]);
                }
            Log.i("QHEngine","Num config match: " + numConfigs[0]);
            }
            return configs[0];
        }
    }
}
