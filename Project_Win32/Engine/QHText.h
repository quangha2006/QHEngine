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
	float Alpha;
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
	void setPos(glm::ivec2 newPos);
	void setColor(glm::vec3 newColor);
	void setAlpha(float newAlpha);
	void setScale(float newScale);
	glm::vec3 getColor(glm::vec3 newColor);
	float getAlpha(float newAlpha);
	float getScale(float newScale);
	glm::ivec2 getEndPos();
	int getEndPos_x();
	int getEndPos_y();
	std::vector<TextData> getTextData();
	void UpdateId(int newid);
	QHText(std::string text = "", int pos_x = 0.0f, int pos_y = 0.0f, glm::vec3 color = glm::vec3(1.0f), float scale = 1.0f, float alpha = 1.0f);
	~QHText();
};

