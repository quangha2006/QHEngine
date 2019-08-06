#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "QHTexture.h"
#include "Mesh.h"

class QHMaterial
{
private:

public:
	glm::vec3 mAmbient;
	glm::vec3 mDiffuse;
	glm::vec3 mSpecular;
	float mShininess;
	float mTransparent;
	bool mHasNormals;
	GLuint mIndices_index;
	GLuint mIndices_size;
	std::vector<Texture> mTextures;
	void Apply(RenderMode mode, bool isEnableAlpha = false);
	QHMaterial();
	~QHMaterial();
};

