#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__

#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Sprite.h"
#include <vector>

class UserInterface
{
private:
	static UserInterface* instance;
	Shader mShader;
	glm::mat4 mProjection;
	std::vector<Sprite*> mListUI;
	GLuint mVAO, mVBO;
	float mVertexData[4];
	int mWindowsWidth, mWindowsHeight;
public:
	static UserInterface* getInstance();
	static Sprite *CreateWithTexture(const char * path);
	bool Init(int width, int height);
	void Render();
	void AddSprite(Sprite *sprite);
	bool OnTouchEvent(int eventId, int x, int y, int pointerId);
	UserInterface();
	~UserInterface();
};

#endif //!__USERINTERFACE_H__