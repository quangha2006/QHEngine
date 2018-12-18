#include <iostream>
#include "AppBase.h"
#include "Timer.h"
#include "Logs.h"
#include <math.h>
#include <thread>
#include <android/native_window_jni.h> //for ANativeWindow
#include <android/native_window.h> // requires ndk r5 or newer
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <float.h>
AppBase *app = NULL;
struct TouchPoint
{
	int x;
	int y;
};
void Init_MainAndroid(int w, int h, ANativeWindow * window)
{
	app = AppFactory();
	//app->GetContext()->SetWindow(window);
	if (!app->initialize(w, h))
	{
		LOGE("FAILED initialize");
	}
}
void Resize(int w, int h) 
{
	app->Resize(w,h);
}

void Update(long deltaTime) 
{
	app->rendering();
}

void CleanUp() {}

void OnGameTouchEvent(int eventId, int x, int y, int pointerId) 
{
	static int numtouch = 0;
	static TouchPoint currenttouchpoint[10];
	static float disold = 0.0f;
	double yoffset = 0.0f;
	if (eventId == 0) numtouch++;
	if (eventId == 2) numtouch--;

	currenttouchpoint[pointerId].x = x;
	currenttouchpoint[pointerId].y = y;

	if (numtouch >=2)
	{
		//app->OnGameTouchEvent(2, currenttouchpoint[0].x, currenttouchpoint[0].y, 0);

		int dis_x = currenttouchpoint[0].x - currenttouchpoint[1].x;
		int dis_y = currenttouchpoint[0].y - currenttouchpoint[1].y;

		float dis = sqrt(pow(dis_x,2) + pow(dis_y,2));
		if (disold < 0.0001f)
			disold = dis;
		else
		{
			yoffset = (dis - disold) / 10.0f;// > 0.0f) ? 1.0f : -1.0f;
			disold = dis;
		}
			
	}

	switch (numtouch)
	{
		case 1:
			app->OnGameTouchEvent(eventId, currenttouchpoint[0].x, currenttouchpoint[0].y, 0);
			disold = 0.0f;
			break;
		case 2:
			//LOGI("yoffset: %f", (float)yoffset);
			app->ZoomCamera(0, yoffset);
			break;
		default:
			break;
	}
}
//#endif