#pragma once
#include <glm/glm.hpp>
using namespace glm;
class Camera
{
private:
	static Camera* instance;
public:
	float zoom;
	float View_near;
	float View_far;
	vec3 Pos;
	vec3 Target;
	vec3 up;
	vec3 Front;

	vec3 lightPos;
	float light_near;
	float light_far;

	vec3 Direction();
	vec3 Up();
	vec3 Right();

	mat4 view;
	mat4 projection;
	mat4 WorldViewProjectionMatrix;

	mat4 lightProjection;
	mat4 lightView;
	mat4 lightSpaceMatrix;

	vec3 ExtractCameraPos(const glm::mat4 & a_modelView);
	void UpdateView();
	void UpdateWorldViewProjection();
	static Camera* getInstance();
	Camera();
	~Camera();
};