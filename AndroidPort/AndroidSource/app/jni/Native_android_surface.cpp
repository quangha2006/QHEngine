#include <initializer_list>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <jni.h>
#include <errno.h>
#include <cassert>

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <thread>
#include <android/window.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include "GameFunctionsToImplement.h"
#define LOGI2(...) ((void)__android_log_print(ANDROID_LOG_INFO, "QHEngine", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "QHEngine", __VA_ARGS__))

struct engine {
	struct android_app* app;

	int animating;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;

};

static int engine_init_display(struct engine* engine) {
	/*
	* Here specify the attributes of the desired configuration.
	* Below, we select an EGLConfig with at least 8 bits per color
	* component compatible with on-screen windows
	*/
	const EGLint attribs[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		//EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_ALPHA_SIZE,     8,
		EGL_DEPTH_SIZE,     24,
		//EGL_STENCIL_SIZE,   8,
		//EGL_SAMPLE_BUFFERS, 1,
		//EGL_SAMPLES, 4,
		EGL_NONE
	};
	EGLint w, h, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires.
	* find the best match if possible, otherwise use the very first one
	*/
	eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);
	std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
	assert(supportedConfigs);
	eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);
	assert(numConfigs);
	config = supportedConfigs[0];
	//LOGI2("numConfigs: %d", numConfigs);
	auto i = 0;
	for (; i < numConfigs; i++) {
		auto& cfg = supportedConfigs[i];
		EGLint r, g, b, a, d, s, samb, sam;
		if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) &&
			eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
			eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) &&
			eglGetConfigAttrib(display, cfg, EGL_ALPHA_SIZE, &a) &&
			eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
			eglGetConfigAttrib(display, cfg, EGL_STENCIL_SIZE, &s) &&
			eglGetConfigAttrib(display, cfg, EGL_SAMPLE_BUFFERS, &samb) &&
			eglGetConfigAttrib(display, cfg, EGL_SAMPLES, &sam))/* &&
			r == 8 && g == 8 && b == 8 && d == 0) */{
			//LOGI2("r: %d, g:%d, b:%d, a:%d, d:%d, s:%d, samb:%d, sam:%d", r,g,b,a,d,s,samb,sam);
			if (sam == (EGLint)4) {
				config = supportedConfigs[i];
				//LOGI2("Choose config %d", i);
			}
				
			//break;
		}
	}
	//if (i == numConfigs) {
	//	LOGI2("Choose config 0");
	//	config = supportedConfigs[0];
	//}

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	* As soon as we picked a EGLConfig, we can safely reconfigure the
	* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	const EGLint context_attrib_list[] = {
		// request a context using Open GL ES 2.0
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};

	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
	surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attrib_list);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;

	//glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//glShadeModel(GL_SMOOTH);
	//glDisable(GL_DEPTH_TEST);

	return 0;
}

/**
* Just the current frame in the display.
*/
static void engine_draw_frame(struct engine* engine) {
	if (engine->display == NULL) {
		// No display.
		return;
	}

	Update((long)0);
	eglSwapBuffers(engine->display, engine->surface);
}

/**
* Tear down the EGL context currently associated with the display.
*/
static void engine_term_display(struct engine* engine) {
	//return;
	LOGI2("engine_term_display");

	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}
	engine->animating = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
* Process the next input event.
*/
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {

	struct engine* engine = (struct engine*)app->userData;
	if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION)
		return 0;
	

	const int action = AMotionEvent_getAction(event);
	const int mask = (action & AMOTION_EVENT_ACTION_MASK);
	const int count = AMotionEvent_getPointerCount(event);
	const int pointerId = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

	if (action == AMOTION_EVENT_ACTION_DOWN) {
		OnGameTouchEvent(0, (int)AMotionEvent_getX(event, 0), (int)AMotionEvent_getY(event, 0), 0);
	}

	if (mask == AMOTION_EVENT_ACTION_POINTER_DOWN) {
		OnGameTouchEvent(0, (int)AMotionEvent_getX(event, pointerId), (int)AMotionEvent_getY(event, pointerId), pointerId);
	}

	if (action == AMOTION_EVENT_ACTION_MOVE) {
		for (int i = 0; i < count; i++) {
			const int id = AMotionEvent_getPointerId(event, i);
			const int x = (int)AMotionEvent_getX(event, id);
			const int y = (int)AMotionEvent_getY(event, id);
			OnGameTouchEvent(1, x, y, id);
		}
	}

	if (mask == AMOTION_EVENT_ACTION_POINTER_UP) {
		OnGameTouchEvent(2, (int)AMotionEvent_getX(event, pointerId), (int)AMotionEvent_getY(event, pointerId), pointerId);
	}

	if (action == AMOTION_EVENT_ACTION_UP) {
		for (int i = 0; i < count; i++)
		{
			const int id = AMotionEvent_getPointerId(event, i);
			OnGameTouchEvent(2, (int)AMotionEvent_getX(event, pointerId), (int)AMotionEvent_getY(event, id), pointerId);
		}
	}
	return 1;

}

/**
* Process the next main command.
*/
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		LOGI2("APP_CMD_SAVE_STATE");
		// The system has asked us to save our current state.  Do so.

		break;
	case APP_CMD_INIT_WINDOW:
		LOGI2("APP_CMD_INIT_WINDOW");
		// The window is being shown, get it ready.
		if (engine->app->window != NULL) {
			//engine_init_display(engine);
			static bool initialised = false;
			if (!initialised) {
				engine_init_display(engine);
				Init_MainAndroid(engine->width, engine->height);
				initialised = false;
			}
			else
			{
				if (eglMakeCurrent(engine->display, engine->surface, engine->surface, engine->context) == EGL_FALSE) {
					LOGW("Unable to eglMakeCurrent");
				}
			}

			engine_draw_frame(engine);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		LOGI2("APP_CMD_TERM_WINDOW:");
		// The window is being hidden or closed, clean it up.
		engine_term_display(engine);
		break;
	case APP_CMD_GAINED_FOCUS:
		engine->animating = 1;
		Resize(engine->width, engine->height);
		break;
	case APP_CMD_LOST_FOCUS:
		engine->animating = 0;
		engine_draw_frame(engine);
		break;
	case APP_CMD_WINDOW_RESIZED:
	case APP_CMD_CONFIG_CHANGED:
		LOGI2("Config Change");
		Resize(engine->width, engine->height);
		//SetOrientation();
		break;
	case APP_CMD_PAUSE:
		break;
	case APP_CMD_RESUME:
		break;
	case APP_CMD_DESTROY:
		break;
	}
}

/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
void android_main(struct android_app* state) {

	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);
	ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_DIM_BEHIND, 0);
	//ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_TRANSLUCENT_NAVIGATION, 0);
	ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_FULLSCREEN, 0);
	//ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_SHOW_WHEN_LOCKED, 0);
	//ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_SHOW_WHEN_LOCKED, 0);
	//ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_SHOW_WHEN_LOCKED, 0);
	engine.app = state;

	// loop waiting for stuff to do.

	while (1) {
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
			(void**)&source)) >= 0) {
			// Process this event.
			if (source != NULL) {
				source->process(state, source);
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0) {
				engine_term_display(&engine);
				return;
			}
		}
		if (engine.animating) {
			engine_draw_frame(&engine);
		}
	}
}
//END_INCLUDE(all)
