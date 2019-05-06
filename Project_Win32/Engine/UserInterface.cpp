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

	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	if (mShader.position_Attribute > -1)
	{
		glEnableVertexAttribArray(mShader.position_Attribute);
		glVertexAttribPointer(mShader.position_Attribute, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	}

	if (mShader.TexCoord_Attribute > -1)
	{
		glEnableVertexAttribArray(mShader.TexCoord_Attribute);
		glVertexAttribPointer(mShader.TexCoord_Attribute, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)2);
	}
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW); // max 200 characters
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return true;
}

void UserInterface::Render()
{
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	mShader.use();
	glUniformMatrix4fv(glGetUniformLocation(mShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(mProjection));

	glBindVertexArray(mVAO);

	glActiveTexture(GL_TEXTURE0);
	for (int i = 0; i < mListUI.size(); i++)
	{
		GLuint texid = mListUI[i]->GetTexId();
		glm::vec2 pos = mListUI[i]->GetPos();
		glBindTexture(GL_TEXTURE_2D, texid);
		float texData[]{pos.x, pos.y, }
	}
}

UserInterface::UserInterface()
{
}


UserInterface::~UserInterface()
{
}
