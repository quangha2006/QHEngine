#ifndef __QHMATH_H__
#define __QHMATH_H__

#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace QHMath
{
	glm::mat4 AiToGLMMat4(const aiMatrix4x4 &in_mat);
	glm::mat4 CombineMat4(const glm::mat4 &a, const glm::mat4 &b);
	bool compareFloat(float A, float B, float epsilon = 0.0001f);

	template <class T> void Clamp(T& a, const T& lb, const T& ub)
	{
		if (a < lb)
		{
			a = lb;
		}
		else if (ub < a)
		{
			a = ub;
		}
	}

	template<typename T> inline T clamp(T x, T _min, T _max)
	{
		return math::min(math::max(x, _min), _max);
	}
}

#endif //!__QHMATH_H__