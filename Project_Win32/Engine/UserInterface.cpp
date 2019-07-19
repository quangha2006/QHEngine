#include "UserInterface.h"
#include "Debugging.h"
#include "RenderManager.h"

UserInterface * UserInterface::instance = NULL;

UserInterface * UserInterface::getInstance()
{
	if (instance == NULL)
	{
		instance = new UserInterface();
	}
	return instance;
}

Sprite * UserInterface::CreateWithTexture(const char * path)
{
	Sprite *sprite = new Sprite;
	if (!sprite->LoadTexture(path))
		return nullptr;
	sprite->SetPos(0, 0);
	UserInterface::getInstance()->AddSprite(sprite);
	return sprite;
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
		"uniform float alpha;\n"
		"uniform bool isGray;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 color = texture2D(texture, TexCoords);\n"
		"	if (isGray == true)\n"
		"	{\n"
		"		color.rgb = vec3((color.r + color.g + color.b) / 3.);\n"
		"	}\n"
		"	gl_FragColor = vec4(color.rgb, color.a * alpha);\n"
		"}\n"
	};
	if (!mShader.LoadShader(vtxSrc, fragSrc, true))
		return false;

	mProjection = glm::ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f);
	mWindowsWidth = width;
	mWindowsHeight = height;
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
		glVertexAttribPointer(mShader.TexCoord_Attribute, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), NULL, GL_DYNAMIC_DRAW); // max 200 characters
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
	
	
	glBindVertexArray(mVAO);

	glActiveTexture(GL_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	for (unsigned int i = 0; i < mListUI.size(); i++)
	{
		if (!mListUI[i]) continue;
		
		glm::mat4 projection = mProjection;
		GLuint texid = mListUI[i]->GetTexId();
		glm::vec2 pos = mListUI[i]->GetPos();
		float angle = mListUI[i]->GetRotateAngle();
		int texwidth = mListUI[i]->GetWidth();
		int texheight = mListUI[i]->GetHeight();
		float alpha = mListUI[i]->GetAlpha();
		bool isGrayOut = mListUI[i]->IsGrayOut();

		float texData[16]{
			// positions   // texCoords
			pos.x,				pos.y,				0.0f, 1.0f,
			pos.x,				pos.y + texheight,	0.0f, 0.0f,
			pos.x + texwidth,	pos.y,				1.0f, 1.0f,
			pos.x + texwidth,	pos.y + texheight,	1.0f, 0.0f
		};

		projection = glm::rotate(projection, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

		glUniform1f(glGetUniformLocation(mShader.program, "alpha"), alpha);
		glUniform1f(glGetUniformLocation(mShader.program, "isGray"), isGrayOut);
		glUniformMatrix4fv(glGetUniformLocation(mShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindTexture(GL_TEXTURE_2D, texid);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * sizeof(float), texData);
		QHEngine::DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void UserInterface::AddSprite(Sprite * sprite)
{
	if (sprite)
	{
		mListUI.push_back(sprite);
	}
}

bool UserInterface::OnTouchEvent(int eventId, int x, int y, int pointerId)
{
	for (unsigned int i = 0; i < mListUI.size(); i++)
	{
		if (mListUI[i]->OnTouchEvent(eventId, x, y, pointerId))
			return true;
	}
	return false;
}

UserInterface::UserInterface()
{
}


UserInterface::~UserInterface()
{
}
