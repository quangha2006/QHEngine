// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <Logs.h>

//#include "../Utilities/esUtil.h" // for esLogMessage
#ifndef ANDROID
//#include "targetver.h"
#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <tchar.h>
//#include "../Utilities/esUtil.h" // for esLogMessage
#endif

#ifdef ANDROID
#include <android/log.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#define TRUE 1
#define FALSE 0
#define PLATFORM "OPENGLES"
#endif
inline
GLenum CheckGLError(const char *calling_location)
{
	/*	check for errors	*/
	GLenum err_code = glGetError();
	if (GL_NO_ERROR != err_code)
	{
		std::string error_enum = "";
		switch (err_code)
		{
		case GL_INVALID_ENUM:
			error_enum = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_enum = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error_enum = "GL_INVALID_OPERATION";
			break;
#if defined(_WINDOWS)
		case GL_STACK_OVERFLOW:
			error_enum = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			error_enum = "GL_STACK_UNDERFLOW";
			break;
		case GL_TABLE_TOO_LARGE:
			error_enum = "GL_TABLE_TOO_LARGE";
			break;
#endif
		case GL_OUT_OF_MEMORY:
			error_enum = "GL_OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_enum = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		default:
			error_enum = "unknown";
		}
		LOGI("OpenGL Error @ %s: %s (%i)\n", calling_location, error_enum.c_str(), err_code);
		//err_code = glGetError();
	}
	return err_code;
}
/**
TextRendering::Add(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLfloat alpha)
**/
//#define AddText(...) TextRendering::getInstance()->Add(__VA_ARGS__)
// TODO: reference additional headers your program requires here
