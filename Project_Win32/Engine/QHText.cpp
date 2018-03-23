#include "QHText.h"
#include "TextRendering.h"

bool cmpf(float A, float B, float epsilon = 0.0001f)
{
	return (fabs(A - B) < epsilon);
}
void QHText::MakeTextData()
{
	m_textdata.clear();
	if (text.size() > 0)
		endPos = TextRendering::getInstance()->Add(text, pos.x, pos.y, scale, color, alpha, m_textdata);
}

void QHText::setText(std::string newText)
{
	if (this->text == newText) return;
	this->text = newText;
	MakeTextData();
}

void QHText::setPos(int pos_x, int pos_y)
{
	if (cmpf(this->pos.x , pos_x) && cmpf(this->pos.y , pos_y)) return;
	this->pos.x = pos_x;
	this->pos.y = pos_y;
	MakeTextData();
}

void QHText::setPos(glm::ivec2 newPos)
{
	if (this->pos == newPos) return;
	this->pos = newPos;
	MakeTextData();
}

void QHText::setColor(glm::vec3 newColor)
{
	if (this->color == newColor)
		return;

	this->color = newColor;
	for (int i = 0; i < m_textdata.size(); i++)
	{
		m_textdata[i].Color = newColor;
	}
}

void QHText::setAlpha(float newAlpha)
{
	if (cmpf(this->alpha , newAlpha))
		return;

	this->alpha = newAlpha;
	for (int i = 0; i < m_textdata.size(); i++)
	{
		m_textdata[i].Alpha = newAlpha;
	}
}

void QHText::setScale(float newScale)
{
	if (cmpf(this->scale , newScale))
		return;

	this->scale = newScale;
	MakeTextData();
}

glm::vec3 QHText::getColor(glm::vec3 newColor)
{
	return this->color;
}

GLfloat QHText::getAlpha(float newAlpha)
{
	return this->alpha;
}

GLfloat QHText::getScale(float newScale)
{
	return this->scale;
}

glm::ivec2 QHText::getEndPos()
{
	return this->endPos;
}

int QHText::getEndPos_x()
{
	return this->endPos.x;
}

int QHText::getEndPos_y()
{
	return this->endPos.y;
}

std::vector<TextData> QHText::getTextData()
{
	return m_textdata;
}

void QHText::UpdateId(int newid)
{
	id = newid;
}

QHText::QHText(std::string text, int pos_x, int pos_y, glm::vec3 color, float scale, float alpha)
{
	this->id = TextRendering::getInstance()->AddQHText(this);
	this->text = text;
	this->color = color;
	this->pos = glm::vec2(pos_x, pos_y);
	this->alpha = alpha;
	this->scale = scale;
	MakeTextData();
}


QHText::~QHText()
{
	TextRendering::getInstance()->RemoveQHText(id);
	m_textdata.clear();
}
