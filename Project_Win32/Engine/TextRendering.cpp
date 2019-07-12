#include "stdafx.h"
#include "TextRendering.h"
#include "ShaderManager.h"
#include <SOIL.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "Debugging.h"
#include "Utils.h"
TextRendering * TextRendering::instance = NULL;


TextRendering * TextRendering::getInstance()
{
	if (instance == NULL)
	{
		instance = new TextRendering();
	}
	return instance;
}

void TextRendering::UpdateScreenSize(int w, int h)
{
	projection = glm::ortho(0.0f, static_cast<GLfloat>(w), static_cast<GLfloat>(h), 0.0f);
	screen_width = w;
	screen_height = h;
}

int TextRendering::AddQHText(QHText *textneedrender)
{
	mListQHText.push_back(textneedrender);
	return currentLastId++;
}

void TextRendering::RemoveQHText(int id)
{
	for (unsigned int i = 0; i < mListQHText.size(); i++)
	{
		if (mListQHText[i]->GetId() == id)
		{
			mListQHText.erase(mListQHText.begin() + i);
			return;
		}
	}
}

bool TextRendering::Init(const char * font_path, int width, int height, unsigned int maxchar)
{
	char vtxSrc[] = {
		"#version 100\n"
		"attribute vec4 aPos;\n"
		"attribute vec3 textColor;\n"
		"attribute float alpha;\n"
		"\n"
		"varying vec2 thetexCoord;\n"
		"varying vec3 thetextColor;\n"
		"varying float thealpha;\n"
		"\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"	thetexCoord = aPos.zw;\n"
		"	thetextColor = textColor;\n"
		"	thealpha = alpha;\n"
		"	gl_Position = projection * vec4(aPos.xy, 0.0, 1.0);\n"
		"}\n"
	};

	char fragSrc[] = {
		"#version 100\n"
		"precision highp float;\n"
		"\n"
		"varying vec2 thetexCoord;\n"
		"varying vec3 thetextColor;\n"
		"varying float thealpha;\n"
		"\n"
		"uniform sampler2D texture;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(texture, thetexCoord).r);\n"
		"	gl_FragColor = vec4(thetextColor, thealpha) * sampled;\n"
		"}\n"
	};

	if (mShader.LoadShader(vtxSrc, fragSrc, true))
	{
		mPositionAttribute = glGetAttribLocation(mShader.program, "aPos");
		mColorAttribute = glGetAttribLocation(mShader.program, "textColor");
		mAlphaAttribute = glGetAttribLocation(mShader.program, "alpha");
		mProjectionUniform = glGetUniformLocation(mShader.program, "projection");
		mTextureUniform = glGetUniformLocation(mShader.program, "texture");
	}

	UpdateScreenSize(width, height);

	m_Maxchar = maxchar; // 6 vertexs for each character
	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		LOGE("ERROR::FREETYPE: Could not init FreeType Library\n");
		return false;
	}

	// Load font as face
	FT_Face face;
	std::string path_modif(Utils::getResourcesFolder() + font_path);

	if (FT_New_Face(ft, path_modif.c_str(), 0, &face))
	{
		LOGE("ERROR::FREETYPE: Failed to load font");
		return false;
	}

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);
	// Create a texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Generate texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	// Set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	uint32_t posX = 2;
	uint32_t posY = 2;
	uint32_t maxHeight = 0;
	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			LOGI("ERROR::FREETYTPE: Failed to load Glyph");
			continue;
		}
		
		// Print A Character Into The Texture.
		if (posX + face->glyph->bitmap.width + 2 >= 512)
		{
			posX = 2;
			posY += (maxHeight + 2);
		}
		glTexSubImage2D(GL_TEXTURE_2D, 0, posX, posY, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// Store Info.
		GLuint Advance;
		Advance = face->glyph->advance.x >> 6;     // Advance
		glm::uvec2 Size;
		Size.x = face->glyph->bitmap.width;       // Size
		Size.y = face->glyph->bitmap.rows;        // Size
		glm::ivec2 Bearing;
		Bearing.x = face->glyph->bitmap_left;        // Bearing
		Bearing.y = face->glyph->bitmap_top;         // Bearing
		glm::fvec2 Texcord;
		Texcord.x = (float)posX / (float)512;
		Texcord.y = (float)posY / (float)512;

		if (Advance != 0)
		{
			posX += (face->glyph->bitmap.width + 2);
			maxHeight = maxHeight > face->glyph->bitmap.rows ? maxHeight : face->glyph->bitmap.rows;
		}
		else continue;
		
		// Now store character for later use
		CharInfo character = { Advance, Size, Bearing, Texcord };

		mCharInfo.insert(std::pair<GLchar, CharInfo>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	glEnableVertexAttribArray(mPositionAttribute);
	glVertexAttribPointer(mPositionAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(TextData), (void*)offsetof(TextData, Position));

	glEnableVertexAttribArray(mColorAttribute);
	glVertexAttribPointer(mColorAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(TextData), (void*)offsetof(TextData, Color));

	glEnableVertexAttribArray(mAlphaAttribute);
	glVertexAttribPointer(mAlphaAttribute, 1, GL_FLOAT, GL_FALSE, sizeof(TextData), (void*)offsetof(TextData, Alpha));

	glBufferData(GL_ARRAY_BUFFER, m_Maxchar * 6 * sizeof(TextData), NULL, GL_DYNAMIC_DRAW); // max 200 characters
	glBindVertexArray(0);

	// For debug
	float quadVertices[]{ // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
						  // positions   // texCoords
		0.2f,  1.0f, 0.0f, 0.0f, 1.0f,
		0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f,  0.2f, 0.0f, 1.0f, 0.0f
	};

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), quadVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_initialized = true;
	return true;
}

glm::ivec2 TextRendering::CreateTextData(const char * text, int x, int y, float scale, glm::vec3 color, float alpha, std::vector<TextData> &textdata)
{
	unsigned int text_len = strlen(text);

	if (text_len <= 0) return glm::ivec2(x, y);

	int x_origin = x;
	int y_origin = y;
	float xpos;
	float ypos;
	CharInfo Upper_ch = mCharInfo['A'];
	std::string::const_iterator c;

	for (unsigned int i = 0; i < text_len ; i++)
	{
		CharInfo ch = mCharInfo[text[i]];

		xpos = x + ch.Bearing.x * scale;
		if (xpos  + ch.Advance > this->screen_width)
		{
			xpos = x_origin + ch.Bearing.x * scale;
			x = x_origin;
			y += (int)(Upper_ch.Size.y * scale);
		}
		ypos = y + (Upper_ch.Size.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		float sizew = (float)ch.Size.x / 512.0f;
		float sizeh = (float)ch.Size.y / 512.0f;

		TextData pos1{ glm::vec4(xpos,	 ypos	 , ch.Texcord.x ,		ch.Texcord.y		),	color ,alpha };
		TextData pos2{ glm::vec4(xpos,     ypos + h, ch.Texcord.x ,		ch.Texcord.y + sizeh),	color ,alpha };
		TextData pos3{ glm::vec4(xpos + w, ypos + h, ch.Texcord.x + sizew,ch.Texcord.y + sizeh),color ,alpha };
		TextData pos4{ glm::vec4(xpos,     ypos	 , ch.Texcord.x ,		ch.Texcord.y		),	color ,alpha };
		TextData pos5{ glm::vec4(xpos + w, ypos + h, ch.Texcord.x + sizew,ch.Texcord.y + sizeh),color ,alpha };
		TextData pos6{ glm::vec4(xpos + w, ypos	 , ch.Texcord.x + sizew,ch.Texcord.y		),	color ,alpha };

		textdata.push_back(pos1);
		textdata.push_back(pos2);
		textdata.push_back(pos3);
		textdata.push_back(pos4);
		textdata.push_back(pos5);
		textdata.push_back(pos6);

		x += (int)(ch.Advance * scale);
	}
	// pos for next text
	char text_end = text[text_len - 1];
	CharInfo ch = mCharInfo[text_end];

	if (xpos + ch.Advance * scale > this->screen_width)
	{
		xpos = (float)x_origin;
		ypos += ch.Size.y * scale;
	}
	else
		xpos += ch.Advance * scale;
	return glm::ivec2(xpos, ypos);
}

void TextRendering::Draw()
{
	if (mShader.program == -1 || !m_initialized || mListQHText.size() <= 0) return;

	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	mShader.use();

	glUniformMatrix4fv(mProjectionUniform, 1, GL_FALSE, glm::value_ptr(projection));
	
	glBindVertexArray(mVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	//glUniform1i(mTextureUniform, 0);
	// Update content of VBO memory
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	
	fulltextdata.clear();
	for (auto &ListQHText : mListQHText)
	{
		if (ListQHText->getIsVisible() == false) continue;

		vector<TextData> tmp = ListQHText->getTextData();
		if (tmp.size() > 0)
			fulltextdata.insert(fulltextdata.end(), tmp.begin(), tmp.end());
	}
	if (fulltextdata.size() <= 0) return;
	if (fulltextdata.size() > m_Maxchar * 6)
	{
		glBufferData(GL_ARRAY_BUFFER, fulltextdata.size() * sizeof(TextData), NULL, GL_DYNAMIC_DRAW); // max 200 characters
		m_Maxchar = fulltextdata.size() / 6;
		LOGW("Increase m_Maxchar: %d", m_Maxchar);
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, fulltextdata.size() * sizeof(TextData), &fulltextdata[0]); // Be sure to use glBufferSubData and not glBufferData

	//QHEngine::DrawArrays(GL_TRIANGLES, 0, m_TextData.size()); // for debug to add num draw call and num triangles
	glDrawArrays(GL_TRIANGLES, 0, fulltextdata.size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
	CheckGLError("Draw Text ");
}

TextRendering::TextRendering()
	: screen_width(960)
	, screen_height(540)
	, m_initialized(false)
	, currentLastId(0)
	, mPositionAttribute(-1)
	, mColorAttribute(-1)
	, mAlphaAttribute(-1)
	, mProjectionUniform(-1)
	, mTextureUniform(-1)
{
}

TextRendering::~TextRendering()
{
	glDeleteBuffers(1, &mVBO);
}
