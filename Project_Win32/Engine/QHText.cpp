#include "QHText.h"
#include "TextRendering.h"

bool cmpf(float A, float B, float epsilon = 0.0001f)
{
	return (fabs(A - B) < epsilon);
}
void QHText::MakeTextData()
{
	m_textdata.clear();
	if (m_text.size() > 0)
		m_endPos = TextRendering::getInstance()->Add(m_text, m_pos.x, m_pos.y, m_scale, m_color, m_alpha, m_textdata);
}

void QHText::setText(std::string newText)
{
	if (m_text == newText) return;
	m_text = newText;
	MakeTextData();
}

void QHText::setPos(int pos_x, int pos_y)
{
	if (cmpf(m_pos.x , pos_x) && cmpf(m_pos.y , pos_y)) return;
	m_pos.x = pos_x;
	m_pos.y = pos_y;
	MakeTextData();
}

void QHText::setPos(glm::ivec2 newPos)
{
	if (m_pos == newPos) return;
	m_pos = newPos;
	MakeTextData();
}

void QHText::setColor(glm::vec3 newColor)
{
	if (m_color == newColor)
		return;

	m_color = newColor;
	for (int i = 0; i < m_textdata.size(); i++)
	{
		m_textdata[i].Color = newColor;
	}
}

void QHText::setAlpha(float newAlpha)
{
	if (cmpf(m_alpha , newAlpha))
		return;

	m_alpha = newAlpha;
	for (int i = 0; i < m_textdata.size(); i++)
	{
		m_textdata[i].Alpha = newAlpha;
	}
}

void QHText::setScale(float newScale)
{
	if (cmpf(m_scale , newScale))
		return;

	m_scale = newScale;
	MakeTextData();
}

glm::vec3 QHText::getColor(glm::vec3 newColor)
{
	return m_color;
}

GLfloat QHText::getAlpha(float newAlpha)
{
	return m_alpha;
}

GLfloat QHText::getScale(float newScale)
{
	return m_scale;
}

glm::ivec2 QHText::getEndPos()
{
	return m_endPos;
}

int QHText::getEndPos_x()
{
	return m_endPos.x;
}

int QHText::getEndPos_y()
{
	return m_endPos.y;
}

std::vector<TextData> QHText::getTextData()
{
	return m_textdata;
}

void QHText::UpdateId(int newid)
{
	m_id = newid;
}

QHText::QHText(std::string text, int pos_x, int pos_y, glm::vec3 color, float scale, float alpha)
{
	m_id = TextRendering::getInstance()->AddQHText(this);
	m_text = text;
	m_color = color;
	m_pos = glm::vec2(pos_x, pos_y);
	m_alpha = alpha;
	m_scale = scale;
	visible = true;
	MakeTextData();
}


QHText::~QHText()
{
	TextRendering::getInstance()->RemoveQHText(m_id);
}
