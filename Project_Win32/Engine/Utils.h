#pragma once
#include "stdafx.h"
#include <iostream>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Utils
{
	template< typename... Args > std::string toString(const char* format, Args... args)
	{
		int length = std::snprintf(nullptr, 0, format, args...);
		assert(length >= 0);

		char* buf = new char[length + 1];
		std::snprintf(buf, length + 1, format, args...);
		std::string str(buf);
		delete[] buf;
		return std::move(str);
	}
	std::string getResourcesFolder();
	std::string getDefineVersionShader();
	void PrintMat4(glm::mat4 mat4);
	glm::vec3 RandomColor();
}