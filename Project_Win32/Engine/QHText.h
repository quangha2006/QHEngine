#ifndef __QHTEXT_H__
#define __QHTEXT_H__

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
	size_t m_textLen;
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
	std::string newText = Utils::toString(format, args...);
	
	if (newText.length() == 0) return;

	if (m_text != nullptr && std::string(m_text) == newText)
	{
		return;
	}
	size_t newTextLen = newText.length();
	if (m_textLen != newTextLen)
	{
		m_textLen = newTextLen;

		m_text = (char*)realloc(m_text, (m_textLen + 1) * sizeof(char));
	}

	strcpy(m_text, newText.c_str());
	m_text[m_textLen] = '\0';

	MakeTextData();
}
#endif //!__QHTEXT_H__