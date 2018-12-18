#include "EGLAppContext.h"
#include "Logs.h"
#include <thread>
#include <time.h>
#include <unistd.h>

void ThreadDemo(EGLDisplay eglDisplay, EGLSurface eglSurface_draw, EGLSurface eglSurface_read, EGLContext share_context1)
{

	bool MakeCurrent = eglMakeCurrent(eglDisplay, eglSurface_draw, eglSurface_read, share_context1);
	GLenum err_code = glGetError();
	if (GL_NO_ERROR != err_code)
	{
		LOGI("OpenGL Error 3 @ : %i\n", err_code);
		//err_code = glGetError();
	}
	LOGI("MakeCurrent: %d", MakeCurrent);
	LOGI("=====================================================");
	LOGI("GL Renderer  : %s", glGetString(GL_RENDERER));
	LOGI("GL Version   : %s", glGetString(GL_VERSION));
	LOGI("GL Vendor    : %s", glGetString(GL_VENDOR));
	LOGI("=====================================================\n");
	LOGI("END THREAD!\n");
}
bool EGLAppContext::createWindow(int32_t width, int32_t height)
{
//	LOGI("EGLAppContext::createWindow: %d %d", width, height);
	this->width = width;
	this->height = height;
//	return true;
	EGLContext context = eglGetCurrentContext();
	EGLDisplay display = eglGetCurrentDisplay();
	EGLSurface surface_read = eglGetCurrentSurface(EGL_READ);
	EGLSurface surface_draw = eglGetCurrentSurface(EGL_DRAW);
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
	EGLConfig config;
	EGLint numConfigs;
	EGLint *attribList = new EGLint[21]
	{
		EGL_LEVEL, 0,
		EGL_RENDERABLE_TYPE, 4,
		EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
		EGL_RED_SIZE,       8,
		EGL_GREEN_SIZE,     8,
		EGL_BLUE_SIZE,      8,
		EGL_ALPHA_SIZE,     8,
		EGL_DEPTH_SIZE,     24,
		EGL_SAMPLE_BUFFERS, 1,
		EGL_SAMPLES, 4,
		EGL_NONE
	};
	eglChooseConfig(display, attribList, &config, 1, &numConfigs);
	EGLContext share_context1 = eglCreateContext(display, config, context, contextAttribs);
	if (share_context1 == EGL_NO_CONTEXT)
		LOGE("ERROR! EGL_NO_CONTEXT\n");
	if (display == EGL_NO_DISPLAY)
		LOGI("ERROR! EGL_NO_DISPLAY\n");
	if (surface_draw == EGL_NO_SURFACE)
		LOGE("ERROR! EGL_NO_SURFACE draw\n");
	if (surface_read == EGL_NO_SURFACE)
		LOGE("ERROR! EGL_NO_SURFACE read\n");
	GLenum err_code = glGetError();
	if (GL_NO_ERROR != err_code)
	{
		LOGI("OpenGL Error 3 @ : %i\n", err_code);
		//err_code = glGetError();
	}
	EGLSurface surface2 = eglCreatePbufferSurface(display, config, attribList);
	//eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	//eglMakeCurrent(display, surface_draw, surface_read, share_context1);
	std::thread *abc = new std::thread(ThreadDemo, display, surface2, surface2, share_context1);
	//abc->join();
	//eglMakeCurrent(display, surface_draw, surface_read, context);
	LOGI("=====================================================");
	LOGI("GL Renderer  : %s", glGetString(GL_RENDERER));
	LOGI("GL Version   : %s", glGetString(GL_VERSION));
	LOGI("GL Vendor    : %s", glGetString(GL_VENDOR));
	LOGI("=====================================================\n");

	return true;
	
	EGLint majorVersion;
	EGLint minorVersion;
	//EGLDisplay display;
	//EGLContext context;
	EGLSurface surface;
	//EGLConfig config;
	//EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };



	display = eglGetDisplay(EGL_DEFAULT_DISPLAY); // Default display is used.
	if (display == EGL_NO_DISPLAY)
	{
		LOGI("eglGetDisplay failed.");
	}
	eglInitialize(display, &majorVersion, &minorVersion);
	// Get configs
	if (!eglGetConfigs(display, NULL, 0, &numConfigs))
	{
		LOGI("eglGetConfigs: FALSE");
		//return EGL_FALSE;
	}

	// Choose config
	if (!eglChooseConfig(display, attribList, &config, 1, &numConfigs))
	{
		LOGI("eglChooseConfig: FALSE");
		//return EGL_FALSE;
	}

	// Create a surface
	

	// Create a GL context
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
	if (context == EGL_NO_CONTEXT)
	{
		LOGE("ERROR! eglCreateContext Failed\n");
		//return EGL_FALSE;
	}

	if (window == NULL) LOGE("window == NULL eglCreateWindowSurface");
	surface = eglCreateWindowSurface(display, config, window, NULL);
	//surface = eglCreatePbufferSurface(display, config, attribList);
	if (surface == EGL_NO_SURFACE)
	{
		delete[] attribList;
		LOGE("ERROR! eglCreateWindowSurface Failed\n");
		//return EGL_FALSE;
	}

	// Make the context current
	//eglSwapInterval(display, 0);
	if (!eglMakeCurrent(display, surface, surface, context))
	{
		LOGE("ERROR! eglMakeCurrent Failed\n");
		//return EGL_FALSE;
	}

	if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE)
	{
		LOGE("eglBindAPI FAILED\n");
		//return false;
	}


	LOGI("=====================================================");
	LOGI("GL Renderer  : %s", glGetString(GL_RENDERER));
	LOGI("GL Version   : %s", glGetString(GL_VERSION));
	LOGI("GL Vendor    : %s", glGetString(GL_VENDOR));
	LOGI("=====================================================\n");

	mDisplay = display;
	mSurface = surface;
	Initializee = true;
	return true;
}


void EGLAppContext::SwapBuffers()
{
	if (Initializee) {
		eglSwapBuffers(mDisplay, mSurface);
	}
}

EGLAppContext::EGLAppContext()
{
	Initializee = false;
	this->instance = this;
}

EGLAppContext::~EGLAppContext()
{
}
