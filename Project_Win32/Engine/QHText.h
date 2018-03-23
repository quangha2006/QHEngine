#pragma once
#include "stdafx.h"
#include <vector>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct TextData {
	glm::vec4 Position;
	glm::vec3 Color;
	GLfloat Alpha;
};

class QHText
{
private:
	std::vector<TextData> m_textdata;
	std::string text;
	glm::vec3 color;
	glm::ivec2 pos;
	glm::ivec2 endPos;
	float alpha;
	float scale;
	int id;
	void MakeTextData();
public:
	void setText(std::string newText);
	void setPos(int pos_x, int pos_y);
	void setColor(glm::vec3 newColor);
	void setAlpha(GLfloat newAlpha);
	void setScale(GLfloat newScale);
	glm::vec3 getColor(glm::vec3 newColor);
	GLfloat getAlpha(GLfloat newAlpha);
	GLfloat getScale(GLfloat newScale);
	std::vector<TextData> getTextData();
	void UpdateId(int newid);
	QHText(std::string text = "", GLfloat pos_x = 0.0f, GLfloat pos_y = 0.0f, glm::vec3 color = glm::vec3(1.0f), GLfloat scale = 1.0f, GLfloat alpha = 1.0f);
	~QHText();
};

