#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include "AppBase.h"
#include "AppContext.h"
#include "Logs.h"
#include "stdafx.h"
#include <conio.h>
#include <io.h>
#include <Windows.h>

AppBase *app = NULL;
bool lbutton_down = false;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow *window);

std::string current_working_directory()
{
	char working_directory[MAX_PATH + 1];
	GetCurrentDirectoryA(sizeof(working_directory), working_directory); // **** win32 specific ****
	return working_directory;
}
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	freopen("CONOUT$", "w+", stdout);
	std::string currentpath = current_working_directory();
	LOGI("Current Directory: %s", currentpath.c_str());
	app = AppFactory();

	int32_t defaultWidth = 1024;
	int32_t defaultHeight = 520;

	app->GetRequireScreenSize(defaultWidth, defaultHeight);

	if (!app->initialize(defaultWidth, defaultHeight))
	{
		LOGI("FAILED initialize");
		_getch();
		return -1;
	}
	
	AppContext *mContext = app->GetContext();

	glfwSetMouseButtonCallback(mContext->GetWindow(), mouse_button_callback);
	glfwSetCursorPosCallback(mContext->GetWindow(), mouse_pos_callback);
	glfwSetFramebufferSizeCallback(mContext->GetWindow(), framebuffer_size_callback);
	glfwSetScrollCallback(mContext->GetWindow(), scroll_callback);

	do
	{
		app->rendering();
		mContext->SwapBuffers();
	} while (glfwGetKey(mContext->GetWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(mContext->GetWindow()) == 0);
	glfwDestroyWindow(mContext->GetWindow());
	glfwTerminate();
	return 0;
}

void PlatformLog(int type, const char* fmt, ...)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	switch(type)
	{
		case 2:
			SetConsoleTextAttribute(hConsole, 12);
			break;
		case 1:
			SetConsoleTextAttribute(hConsole, 10);
			break;
		default:
			SetConsoleTextAttribute(hConsole, 7);
	}
	const int length = 10240;
	char buffer[length];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf_s(buffer, sizeof(buffer), fmt, ap);
	OutputDebugString(buffer); // need change "Character set" to not set if error
	//OutputDebugString("\n");  // Or change to OutputDebugStringA
	fprintf(stdout, "%s", buffer);
	va_end(ap);
}
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (app == NULL) return;
	
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		double xpos;
		double ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		if (GLFW_PRESS == action)
		{
			lbutton_down = true;
			app->OnGameTouchEvent(0, (int)xpos, (int)ypos, 0);
		}
		else if (GLFW_RELEASE == action)
		{
			lbutton_down = false;
			app->OnGameTouchEvent(2, (int)xpos, (int)ypos, 0);
		}
	}
}
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (app == NULL) return;
	if (lbutton_down)
		app->OnGameTouchEvent(1, (int)xpos, (int)ypos, 0);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	if (width != 0 && height != 0)
		app->Resize(width, height);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	app->ZoomCamera(xoffset, yoffset);
}
