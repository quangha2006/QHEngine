#include "GLFWContext.h"
#include "Logs.h"
#include <thread>

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

	window = glfwCreateWindow(width, height, "QHEngine", NULL, NULL); // Open a window and create its OpenGL context

	if (window == NULL) {
		LOGE("Failed to open GLFW window.");
		glfwTerminate();
		return false;
	}

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
	
	return true; 
}
void GLFWContext::SwapInterval(int interval)
{
	glfwSwapInterval(interval);
}
AppSharedContext *GLFWContext::CreateShareContext()
{
	AppSharedContext *shared_context = new GLFWSharedContext();
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	shared_context->SetWindows(glfwCreateWindow(width, height, "", 0, window));
	return shared_context;
}

void GLFWContext::DestroyContext()
{
	glfwMakeContextCurrent(NULL);
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

void GLFWSharedContext::SetWindows(ANativeWindow * window)
{
	this->window = window;
}

bool GLFWSharedContext::MakeContextCurrent()
{
	glfwMakeContextCurrent(window);
	return true;
}

void GLFWSharedContext::DestroyContext()
{
	glfwMakeContextCurrent(NULL);
}
