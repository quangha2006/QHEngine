#pragma once
#include "stdafx.h"
#include <vector>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Utils.h"

struct TextData {
	glm::vec4 Position;
	glm::vec3 Color;
	float Alpha;
};

class QHText
{
private:
	std::vector<TextData> m_textdata;
	char *m_text;
	unsigned int m_textLen;
	glm::vec3 m_color;
	glm::ivec2 m_pos;
	glm::ivec2 m_endPos;
	float m_alpha;
	float m_scale;
	int m_id;
	bool m_isVisible;
	void MakeTextData();
public:
	void setPos(int pos_x, int pos_y);
	void setPos(glm::ivec2 newPos);
	void setColor(glm::vec3 newColor);
	void setAlpha(float newAlpha);
	void setScale(float newScale);
	void setVisible(bool isvisible);
	glm::vec3 getColor(glm::vec3 newColor);
	float getAlpha(float newAlpha);
	float getScale(float newScale);
	bool getIsVisible();
	glm::ivec2 getEndPos();
	int getEndPos_x();
	int getEndPos_y();
	std::vector<TextData> getTextData();
	void UpdateId(int newid);
	int GetId();
	QHText(const char * text = "", int pos_x = 0.0f, int pos_y = 0.0f, glm::vec3 color = glm::vec3(1.0f), float scale = 1.0f, float alpha = 1.0f);
	~QHText();

	template< typename... Args >
	void setText(const char * format, Args... args);
};

template<typename ...Args>
inline void QHText::setText(const char * format, Args ...args)
{
	char * newText = Utils::toString(format, args...);
	
	if (newText == nullptr) return;

	if (m_text != nullptr && strcmp(m_text, newText) == 0)
	{
		delete[] newText;
		return;
	}
	unsigned int newTextLen = strlen(newText);
	if (m_textLen != newTextLen)
	{
		m_textLen = newTextLen;

		m_text = (char*)realloc(m_text, (m_textLen + 1) * sizeof(char));
	}

	strcpy(m_text, newText);
	m_text[m_textLen] = '\0';

	MakeTextData();
	delete[] newText;
}