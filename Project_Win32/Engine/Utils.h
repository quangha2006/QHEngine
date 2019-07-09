#ifndef __UTILS_H__
#define __UTILS_H__

#include "stdafx.h"
#include <iostream>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Utils
{
	template< typename... Args > char * toString(const char* format, Args... args)
	{
		int length = std::snprintf(nullptr, 0, format, args...);
		assert(length >= 0);

		char* buf = nullptr;
		buf = new char[length + 1];
		std::snprintf(buf, length + 1, format, args...);
		return buf;
	}
	std::string getResourcesFolder();
	std::string getDefineVersionShader();
	void PrintMat4(glm::mat4 &mat4);
	glm::vec3 RandomColor();
}

#endif //!__UTILS_H__