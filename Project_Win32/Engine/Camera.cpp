#include "stdafx.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera * Camera::instance = NULL;

vec3 Camera::Direction()
{
	return normalize(Pos - Target);
}

vec3 Camera::Up()
{
	return cross(Direction(), Right());
}

vec3 Camera::Right()
{
	return normalize(cross(up, Direction()));
}

Camera::Camera()
	: Pos(vec3(0.0f, 3.0f, 8.0f))
	, Target(vec3(0.0f, 1.0f, 0.0f))
	, up(vec3(0.0f, 1.0f, 0.0f))
	, Front(vec3(0.0f, 0.0f, -1.0f))
	, view(glm::mat4(0.0f))
	, projection(glm::mat4(0.0f))
	, zoom(45.0f)
	, View_near(0.1f)
	, View_far(100000.0f)
	, light_near(-20.1f)
	, light_far(80.0f)
{
}
vec3 Camera::ExtractCameraPos(const glm::mat4 & a_modelView)
{
	glm::mat4 modelViewT = transpose(a_modelView);

	// Get plane normals 
	glm::vec3 n1(modelViewT[0]);
	glm::vec3 n2(modelViewT[1]);
	glm::vec3 n3(modelViewT[2]);

	// Get plane distances
	float d1(modelViewT[0].w);
	float d2(modelViewT[1].w);
	float d3(modelViewT[2].w);

	// Get the intersection of these 3 planes
	glm::vec3 n2n3 = cross(n2, n3);
	glm::vec3 n3n1 = cross(n3, n1);
	glm::vec3 n1n2 = cross(n1, n2);

	glm::vec3 top = (n2n3 * d1) + (n3n1 * d2) + (n1n2 * d3);
	float denom = dot(n1, n2n3);

	return top / -denom;
}

void Camera::UpdateWorldViewProjection()
{
	view = glm::lookAt(Pos, Target, up);
	WorldViewProjectionMatrix = projection * view;

	lightProjection = glm::ortho(-80.0f, 80.0f, -40.0f, 40.0f, light_near, light_far);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0, 1.0, 0.2), glm::vec3(0.0, 1.0, 0.2));
	lightSpaceMatrix = lightProjection * lightView;
}
Camera * Camera::getInstance()
{
	if (instance == NULL)
	{
		instance = new Camera();
	}
	return instance;
}
Camera::~Camera()
{
}