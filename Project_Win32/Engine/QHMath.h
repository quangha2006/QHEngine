#ifndef __QHMATH_H__
#define __QHMATH_H__

#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <math.h>
#include "btBulletDynamicsCommon.h"

namespace QHMath
{
	glm::mat4 AiToGLMMat4(const aiMatrix4x4 &in_mat);
	btTransform GLMMAT4ToBLTransform(const glm::mat4 &data);
	bool compareFloat(float A, float B, float epsilon = 0.0001f);
	glm::vec3 GetScale(const glm::mat4 &data);
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
		return std::min(std::max(x, _min), _max);
	}
}

#endif //!__QHMATH_H__