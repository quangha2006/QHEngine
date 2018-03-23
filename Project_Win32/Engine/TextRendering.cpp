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

bool TextRendering::createProgram()
{
	GLint linked = GL_FALSE;
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

	GLuint vertexShader = createShader(GL_VERTEX_SHADER, vtxSrc);
	if (!vertexShader)
	{
		return false;
	}
	char fragSrc[] = {
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
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
	if (!fragmentShader)
	{
		return false;
	}
	program = glCreateProgram();
	if (!program) {
		return false;
	}
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		LOGI("Could not link program\n");
		GLint infoLogLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen) {
			GLchar* infoLog = (GLchar*)malloc(infoLogLen);
			if (infoLog) {
				glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
				LOGI("Could not link program:\n%s\n", infoLog);
				free(infoLog);
			}
		}
		glDeleteProgram(program);
		program = 0;
	}
	return true;
}

GLuint TextRendering::createShader(GLenum shaderType, const char * src)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) {
		return 0;
	}
	glShaderSource(shader, 1, &src, NULL);

	GLint compiled = GL_FALSE;

	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLogLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen > 0) {
			GLchar* infoLog = (GLchar*)malloc(infoLogLen);
			if (infoLog) {
				glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
				LOGI("Could not compile %s shader:\n%s\n", shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
				free(infoLog);
			}
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

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
#if defined(ANDROID)
	float scale = 900.0f / w;
	if (w > 900) w = 900;
	h = h * scale;
#endif
	projection = glm::ortho(0.0f, static_cast<GLfloat>(w), static_cast<GLfloat>(h), 0.0f);
	this->screen_width = w;
	this->screen_height = h;
}

int TextRendering::AddQHText(QHText *textneedrender)
{
	m_ListQHText.push_back(textneedrender);
	return m_ListQHText.size() - 1;
}

void TextRendering::RemoveQHText(int id)
{
	if (id >= 0 && id < m_ListQHText.size())
		m_ListQHText.erase(m_ListQHText.begin() + id);
	// Update id for older text
	if (id < m_ListQHText.size())
	{
		for (int i = id; i < m_ListQHText.size(); i++)
		{
			m_ListQHText[i]->UpdateId(i);
		}
	}
}

bool TextRendering::Init(const char * font_path, int width, int height, unsigned int maxchar)
{
	if (createProgram())
	{
		position_Attribute = glGetAttribLocation(program, "aPos");
		color_Attribute = glGetAttribLocation(program, "textColor");
		alpha_Attribute = glGetAttribLocation(program, "alpha");
	}
	UpdateScreenSize(width, height);
	m_Maxchar = maxchar; // 6 vertexs for each character
	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		LOGI("ERROR::FREETYPE: Could not init FreeType Library\n");
		return false;
	}

	// Load font as face
	FT_Face face;
	std::string path_modif(Utils::getResourcesFolder() + font_path);

	if (FT_New_Face(ft, path_modif.c_str(), 0, &face))
	{
		LOGI("ERROR::FREETYPE: Failed to load font");
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
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_LUMINANCE,
		512,
		512,
		0,
		GL_LUMINANCE,
		GL_UNSIGNED_BYTE,
		0
	);
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
		glTexSubImage2D(GL_TEXTURE_2D, 0, posX, posY, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_LUMINANCE, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
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

		m_CharInfo.insert(std::pair<GLchar, CharInfo>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(position_Attribute);
	glVertexAttribPointer(position_Attribute, 4, GL_FLOAT, GL_FALSE, sizeof(TextData), (void*)offsetof(TextData, Position));
	glEnableVertexAttribArray(color_Attribute);
	glVertexAttribPointer(color_Attribute, 3, GL_FLOAT, GL_FALSE, sizeof(TextData), (void*)offsetof(TextData, Color));
	glEnableVertexAttribArray(alpha_Attribute);
	glVertexAttribPointer(alpha_Attribute, 1, GL_FLOAT, GL_FALSE, sizeof(TextData), (void*)offsetof(TextData, Alpha));
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
	isFontLoaded = true;
	return true;
}

glm::ivec2 TextRendering::Add(std::string text, int x, int y, float scale, glm::vec3 color, float alpha, std::vector<TextData> &textdata)
{
	if (text.length() <= 0) return glm::ivec2(x, y);
	int x_origin = x;
	int y_origin = y;
	GLfloat xpos;
	GLfloat ypos;
	CharInfo Upper_ch = m_CharInfo['A'];
	std::string::const_iterator c;

	for (c = text.begin(); c != text.end(); c++)
	{
		CharInfo ch = m_CharInfo[*c];

		xpos = x + ch.Bearing.x * scale;
		if (xpos  + ch.Advance > this->screen_width)
		{
			xpos = x_origin + ch.Bearing.x * scale;
			x = x_origin;
			y += (Upper_ch.Size.y) * scale;
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

		x += (ch.Advance) * scale;
	}
	// pos for next text
	char text_end = text[text.length() - 1];
	CharInfo ch = m_CharInfo[text_end];

	if (xpos + ch.Advance * scale > this->screen_width)
	{
		xpos = x_origin;
		ypos += ch.Size.y * scale;
	}
	else
		xpos += ch.Advance * scale;
	return glm::ivec2(xpos, ypos);
}

glm::ivec2 TextRendering::Add(std::string text, float number, int x, int y, float scale, glm::vec3 color, float alpha, std::vector<TextData> &m_TextData)
{
	char aaa[50];
	sprintf(aaa, "%.1f", number);
	std::string str = text + std::string(aaa);
	return Add(str, x, y, scale, color, alpha, m_TextData);
}

glm::ivec2 TextRendering::Add(std::string text, int number, int x, int y, float scale, glm::vec3 color, float alpha, std::vector<TextData> &m_TextData)
{
	char aaa[50];
	sprintf(aaa, "%d", number);
	std::string str = text + std::string(aaa);
	return Add(str, x, y, scale, color, alpha, m_TextData);
}

void TextRendering::Draw()
{
	if (program == 0 || !isFontLoaded || m_ListQHText.size() <= 0) return;

	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	// Update content of VBO memory
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	vector<TextData> fulltextdata;
	for (int i = 0; i < m_ListQHText.size(); i++)
	{
		vector<TextData> tmp = m_ListQHText[i]->getTextData();
		fulltextdata.insert(fulltextdata.end(), tmp.begin(), tmp.end());
	}
	if (fulltextdata.size() > m_Maxchar * 6)
	{
		glBufferData(GL_ARRAY_BUFFER, fulltextdata.size() * sizeof(TextData), NULL, GL_DYNAMIC_DRAW); // max 200 characters
		m_Maxchar = fulltextdata.size() / 6;
		LOGI("Increase m_Maxchar: %d", m_Maxchar);
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
{
	program = 0;
	screen_width = 0;
	screen_height = 0;
	isFontLoaded = false;
}

TextRendering::~TextRendering()
{
	glDeleteBuffers(1, &VBO);
}
