#include "GLFWContext.h"
#include "Logs.h"
#include <thread>

//void ThreadDemo(GLFWwindow* win)
//{
//	LOGI("From thread");
//	GLenum err_code = glGetError();
//	if (GL_NO_ERROR != err_code)
//	{
//		LOGI("OpenGL Error 1 @ : %i\n", err_code);
//		//err_code = glGetError();
//	}
//	glfwMakeContextCurrent(win);
//	err_code = glGetError();
//	if (GL_NO_ERROR != err_code)
//	{
//		LOGI("OpenGL Error 2 @ : %i\n", err_code);
//		//err_code = glGetError();
//	}
//	//LOGI("MakeCurrent: %d", MakeCurrent);
//	LOGI("\n=====================================================\n");
//	LOGI("GL Renderer  : %s\n", glGetString(GL_RENDERER));
//	LOGI("GL Version   : %s\n", glGetString(GL_VERSION));
//	LOGI("GL Vendor    : %s\n", glGetString(GL_VENDOR));
//	LOGI("=====================================================\n");
//	glfwDestroyWindow(win);
//	//eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
//}
bool GLFWContext::createWindow(int32_t width, int32_t height) 
{
	if (!glfwInit())
	{
		LOGE("Failed to initialize GLFW\n");
		return false;
	}
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(width, height, "Begin App", NULL, NULL); // Open a window and create its OpenGL context

	if (window == NULL) {
		LOGE("Failed to open GLFW window.");
		glfwTerminate();
		return false;
	}

	//test thread
	//GLFWwindow* win;
	//glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	//win = glfwCreateWindow(width, height, "Optimus example", 0, window);

	//std::thread *qqqqq = new std::thread(ThreadDemo, win);
	//end test thread

	glfwMakeContextCurrent(window); // Initialize GLEW
	glViewport(0, 0, width, height);
	this->width = width;
	this->height = height;
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		LOGE("Failed to initialize GLEW\n");
		return false;
	}
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	LOGI("\n=====================================================\n");
	LOGI("GL Renderer  : %s\n", glGetString(GL_RENDERER));
	LOGI("GL Version   : %s\n", glGetString(GL_VERSION));
	LOGI("GL Vendor    : %s\n", glGetString(GL_VENDOR));
	LOGI("=====================================================\n");
	return true; 
}
void GLFWContext::SwapBuffers()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}
GLFWContext::GLFWContext()
{
	this->instance = this;
}
GLFWContext::~GLFWContext()
{
}
