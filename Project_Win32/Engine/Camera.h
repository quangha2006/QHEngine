#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/glm.hpp>
using namespace glm;
class Camera
{
private:
	static Camera* instance;
	float mZoom;
	float mView_near;
	float mView_far;
	vec3 mPos;
	vec3 mTarget;
	vec3 mUp;
	vec3 mFront;

	vec3 mLightPos;
	float mLight_near;
	float mLight_far;

	mat4 mView;
	mat4 mProjection;
	mat4 mWorldViewProjectionMatrix;

public:
	void SetPos(float posX, float posY, float posZ);
	void SetPos(vec3 pos);
	void SetTarget(float targetX, float targetY, float targetZ);
	void SetTarget(vec3 target);
	void SetLightPos(float lightposX, float lightposY, float lightposZ);
	void SetView(mat4 view);
	void SetProjection(mat4 projection);

	float GetZoom();
	float GetViewNear();
	float GetViewFar();
	float GetLightNear();
	float GetLightFar();
	void GetLightPos(float &posX, float &posY, float &posZ);
	vec3 GetLightPos();
	void GetPos(float &posX, float &posY, float &posZ);
	vec3 GetPos();
	void GetTarget(float &targetX, float &targetY, float &targetZ);
	vec3 GetTarget();
	mat4 GetView() { return mView; };
	mat4 GetProjection() { return mProjection; };
	mat4 GetWorldViewProjectionMatrix() { return mWorldViewProjectionMatrix; };


	vec3 Direction();
	vec3 Up();
	vec3 Right();

	mat4 lightProjection;
	mat4 lightView;
	mat4 lightSpaceMatrix;

	vec3 ExtractCameraPos(const glm::mat4 & a_modelView);
	void UpdateView();
	void UpdateWorldViewProjection();
	void UpdateProjection(int width, int height);
	static Camera* getInstance();
	Camera();
	~Camera();
};
#endif __CAMERA_H__