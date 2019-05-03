#include "UserInterface.h"

UserInterface * UserInterface::instance = NULL;

UserInterface * UserInterface::getInstance()
{
	if (instance == NULL)
	{
		instance = new UserInterface();
	}
	return instance;
}

bool UserInterface::Init(int width, int height)
{
	char vtxSrc[] = {
		"#version 100\n"
		"attribute vec2 aPos;\n"
		"attribute vec2 aTexCoords;\n"
		"\n"
		"varying vec2 TexCoords;\n"
		"\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"	TexCoords = aTexCoords;\n"
		"	gl_Position = projection * vec4(aPos, 0.0, 1.0);\n"
		"}\n"
	};

	char fragSrc[] = {
		"#version 100\n"
		"precision highp float;\n"
		"\n"
		"varying vec2 TexCoords;\n"
		"\n"
		"uniform sampler2D texture;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = texture2D(texture, TexCoords);\n"
		"}\n"
	};
	if (!mShader.LoadShader(vtxSrc, fragSrc, true))
		return false;
	mProjection = glm::ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f);
	return true;
}

void UserInterface::Render()
{
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	mShader.use();

}

UserInterface::UserInterface()
{
}


UserInterface::~UserInterface()
{
}
