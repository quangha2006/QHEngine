#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "QHText.h"
#include "Shader.h"
struct CharInfo {
	GLuint Advance;
	glm::uvec2 Size;
	glm::ivec2 Bearing;
	glm::fvec2 Texcord;
};

class TextRendering
{
private:
	static TextRendering* instance;
	Shader mShader;
	GLuint mVAO, mVBO;

	std::map<GLchar, CharInfo> mCharInfo;

	std::vector<QHText*> mListQHText;

	GLint mPositionAttribute, mColorAttribute, mAlphaAttribute, mProjectionUniform, mTextureUniform;

	glm::mat4 projection;
	GLuint m_TextureID;
	GLuint quadVBO;
	int screen_width, screen_height;
	unsigned int m_Maxchar;

	TextRendering();
	std::vector<TextData> fulltextdata;
	bool m_initialized;
	int currentLastId;
public:
	static TextRendering* getInstance();
	void UpdateScreenSize(int w, int h);
	int AddQHText(QHText *textneedrender);
	void RemoveQHText(int id);
	bool Init(const char * font_path, int width, int height, unsigned int maxchar = 100);
	glm::ivec2 CreateTextData(const char * text, int x, int y, float scale, glm::vec3 color, float alpha, std::vector<TextData> &m_TextData);
	void Draw();
	~TextRendering();
};

