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
	GLuint program;
	GLuint VAO, VBO;;

	std::map<GLchar, CharInfo> m_CharInfo;

	std::vector<QHText*> m_ListQHText;

	GLint position_Attribute, color_Attribute, alpha_Attribute;

	glm::mat4 projection;
	GLuint m_TextureID;
	GLuint quadVBO;
	int screen_width, screen_height;
	unsigned int m_Maxchar;
	bool createProgram();
	GLuint createShader(GLenum shaderType, const char * src);
	TextRendering();
	std::vector<TextData> fulltextdata;
	bool isFontLoaded;
	int currentLastId;
public:
	static TextRendering* getInstance();
	void UpdateScreenSize(int w, int h);
	int AddQHText(QHText *textneedrender);
	void RemoveQHText(int id);
	bool Init(const char * font_path, int width, int height, unsigned int maxchar = 100);
	glm::ivec2 Add(std::string text, int x, int y, float scale, glm::vec3 color, float alpha, std::vector<TextData> &m_TextData);
	glm::ivec2 Add(std::string text, float number, int x, int y, float scale, glm::vec3 color, float alpha, std::vector<TextData> &m_TextData);
	glm::ivec2 Add(std::string text, int number, int x, int y, float scale, glm::vec3 color, float alpha, std::vector<TextData> &m_TextData);
	void Draw();
	~TextRendering();
};

