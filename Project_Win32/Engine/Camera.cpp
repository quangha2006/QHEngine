#include "stdafx.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera * Camera::instance = NULL;

void Camera::SetPos(float posX, float posY, float posZ)
{
	mPos = vec3(posX, posY, posZ);
	UpdateView();
}

void Camera::SetPos(vec3 pos)
{
	mPos = pos;
	UpdateView();
}

void Camera::SetTarget(float targetX, float targetY, float targetZ)
{
	mTarget = glm::vec3(targetX, targetY, targetZ);
	UpdateView();
}

void Camera::SetTarget(vec3 target)
{
	mTarget = target;
	UpdateView();
}

void Camera::SetLightPos(float lightposX, float lightposY, float lightposZ)
{
	mLightPos = vec3(lightposX, lightposY, lightposZ);

	lightView = glm::lookAt(mLightPos, glm::vec3(0.0, 1.0, 0.2), glm::vec3(0.0, 1.0, 0.2));
	lightProjection = glm::ortho(-80.0f, 80.0f, -40.0f, 40.0f, mLight_near, mLight_far);
	lightSpaceMatrix = lightProjection * lightView;
}

void Camera::SetView(mat4 view)
{
	mView = view;
	mPos = ExtractCameraPos(mView);
}

void Camera::SetProjection(mat4 projection)
{
	mProjection = projection;
}

float Camera::GetZoom()
{
	return mZoom;
}

float Camera::GetViewNear()
{
	return mView_near;
}

float Camera::GetViewFar()
{
	return mView_far;
}

float Camera::GetLightNear()
{
	return mLight_near;
}

float Camera::GetLightFar()
{
	return mLight_far;
}

void Camera::GetLightPos(float & posX, float & posY, float & posZ)
{
	posX = mLightPos.x;
	posY = mLightPos.y;
	posZ = mLightPos.z;
}

vec3 Camera::GetLightPos()
{
	return mLightPos;
}

void Camera::GetPos(float & posX, float & posY, float & posZ)
{
	posX = mPos.x;
	posY = mPos.y;
	posZ = mPos.z;
}

vec3 Camera::GetPos()
{
	return mPos;
}

void Camera::GetTarget(float & targetX, float & targetY, float & targetZ)
{
	targetX = mTarget.x;
	targetY = mTarget.y;
	targetZ = mTarget.z;
}

vec3 Camera::GetTarget()
{
	return mTarget;
}

vec3 Camera::Direction()
{
	return normalize(mPos - mTarget);
}

vec3 Camera::Up()
{
	return cross(Direction(), Right());
}

vec3 Camera::Right()
{
	return normalize(cross(mUp, Direction()));
}

Camera::Camera()
	: mPos(vec3(0.0f, 3.0f, 8.0f))
	, mTarget(vec3(0.0f, 1.0f, 0.0f))
	, mUp(vec3(0.0f, 1.0f, 0.0f))
	, mFront(vec3(0.0f, 0.0f, -1.0f))
	, mView(glm::mat4(0.0f))
	, mProjection(glm::mat4(0.0f))
	, mZoom(45.0f)
	, mView_near(0.1f)
	, mView_far(100000.0f)
	, mLight_near(-20.1f)
	, mLight_far(80.0f)
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
	mWorldViewProjectionMatrix = mProjection * mView;
}
void Camera::UpdateView()
{
	mView = glm::lookAt(mPos, mTarget, mUp);
}
void Camera::UpdateProjection(int width, int height)
{
	mProjection = glm::perspective(glm::radians(mZoom), (float)(width) / (float)(height), mView_near, mView_far);
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