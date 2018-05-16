#pragma once
#include <glm/glm.hpp>
using namespace glm;
class Camera
{
public:
	float zoom;
	float View_near;
	float View_far;
	vec3 Pos;
	vec3 Target;
	vec3 up;
	vec3 Front;

	vec3 Direction();
	vec3 Up();
	vec3 Right();

	mat4 view;
	mat4 projection;
	mat4 lookat;
	vec3 ExtractCameraPos(const glm::mat4 & a_modelView);
	void UpdateView();
	Camera();
	~Camera();
};