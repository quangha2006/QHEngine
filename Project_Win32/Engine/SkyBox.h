#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "Shader.h"
#include "Camera.h"
#include <vector>

class SkyBox
{
private:
	Shader mShader;
	GLuint textureID;
	GLfloat *skyboxVertices;
	GLuint mVBO, mVAO;
	glm::mat4 mProjection;

	unsigned int loadCubemap(const char * texturepath, std::vector<std::string> faces);
	GLuint getTextureID();
	bool m_initialized;

public:
	void Init(const char * texturepath);
	void Draw(Camera *camera);
	SkyBox();
	~SkyBox();
};

#endif //!__SKYBOX_H__