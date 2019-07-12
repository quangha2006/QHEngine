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
	glm::mat4 Combinetransformations(const glm::mat4 &a, const glm::mat4 &b);
	bool compareFloat(float A, float B, float epsilon = 0.0001f);
}

#endif //!__QHMATH_H__