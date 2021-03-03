package com.android.quanghaengine;
import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.util.DisplayMetrics;
import android.widget.Toast;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class GLES3View extends GLSurfaceView{

    private static GLES3View mInstance  = null;
    static int widthPixels                  = 0;
    static int heightPixels                 = 0;
    float scaleSurfaceView                  = 0.8f;
    //static Activity activity                = null;
    public GLES3View(Context context) {
    
        super(context);

        mInstance = this;
        //activity = (Activity)context;
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

        setPreserveEGLContextOnPause(true);
        setRenderer(new Renderer());


    }

    public static Context getAppContext() {
        return mInstance.getContext();
    }
    public static synchronized GLES3View getInstance() {
        return mInstance;
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
            GLES3JNILib.InitWithNativeWindow(getInstance().getHolder().getSurface());
        }
    }
    public static String getExternalStorage()
	{
        return getAppContext().getExternalFilesDir(null).getPath();
	}
    public static void ExitApplication(boolean restart)
	{
        //m_bRestartApplicationOnExit = restart;
        //restart: not implement yet
        Activity activity = (Activity)getAppContext();
        activity.runOnUiThread(
			new Runnable() 
			{
				public void run() 
				{
                    Activity activity = (Activity)getAppContext();
                    activity.finish();
				}
			}
		);
	}
    public void onPause()
    {
        KillApplication();
    }
    private void KillApplication() 
	{
		// if(m_bRestartApplicationOnExit) 
		// {
		// 	AlarmManager mgr = (AlarmManager)this.getSystemService(Context.ALARM_SERVICE);
			
		// 	PendingIntent RESTART_INTENT = PendingIntent.getActivity( this, 0, new Intent(this.getIntent()),  Intent.FLAG_ACTIVITY_CLEAR_TASK);
			
		// 	mgr.set(AlarmManager.RTC, System.currentTimeMillis() + 900, RESTART_INTENT);
		// }
		// if(android.os.Build.VERSION.SDK_INT > android.os.Build.VERSION_CODES.ECLAIR_MR1)
		// {
		// 	int pid = android.os.Process.myPid();
		// 	android.os.Process.killProcess(pid); 
		// }
		// else
		// {
		// 	System.exit(0);
		// }
	}
    private static Toast showToastMessage = null;
	public static void ShowToastMessage(final String lang,final int duration)
	{
		if(getAppContext() == null)
			return;
        final Activity activity = (Activity)getAppContext();
		new Thread(new Runnable() { public void run() 
		{
            activity.runOnUiThread(new Runnable() { public void run()
			{
				try{
					if(showToastMessage == null)
					{
						showToastMessage = Toast.makeText(getAppContext(), lang, duration);
					}
					showToastMessage.setText(lang);
					showToastMessage.setDuration(duration);
					android.view.View toastView = showToastMessage.getView();
					if(toastView != null)
					{
						if(toastView.isShown())
						{
							return;
						}
					}
					showToastMessage.show();
				}catch(Exception e){};
			}});
		}}).start();
	}
}
class MyConfigChooser implements GLSurfaceView.EGLConfigChooser {
    @Override
    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
        int[] attribs = {
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