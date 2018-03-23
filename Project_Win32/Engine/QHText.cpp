#include "QHText.h"
#include "TextRendering.h"

bool cmpf(float A, float B, float epsilon = 0.0001f)
{
	return (fabs(A - B) < epsilon);
}
void QHText::MakeTextData()
{
	m_textdata.clear();
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

void QHText::setAlpha(GLfloat newAlpha)
{
	if (cmpf(this->alpha , newAlpha))
		return;

	this->alpha = newAlpha;
	for (int i = 0; i < m_textdata.size(); i++)
	{
		m_textdata[i].Alpha = newAlpha;
	}
}

void QHText::setScale(GLfloat newScale)
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

GLfloat QHText::getAlpha(GLfloat newAlpha)
{
	return this->alpha;
}

GLfloat QHText::getScale(GLfloat newScale)
{
	return this->scale;
}

std::vector<TextData> QHText::getTextData()
{
	return m_textdata;
}

void QHText::UpdateId(int newid)
{
	id = newid;
}

QHText::QHText(std::string text, GLfloat pos_x, GLfloat pos_y, glm::vec3 color, GLfloat scale, GLfloat alpha)
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
