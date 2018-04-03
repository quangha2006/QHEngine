#include "SkyBox.h"



void SkyBox::Init(const char * texturepath)
{
	const char * verShader = {
	"#version 330 core\n"
	"layout(location = 0) in vec3 aPos;\n"
	"out vec3 TexCoords;\n"
	"\n"
	"uniform mat4 projection;\n"
	"uniform mat4 view;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	TexCoords = aPos;\n"
	"	gl_Position = projection * view * vec4(aPos, 1.0);\n"
	"}\n"
	};
	const char *fragShader = {
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"\n"
	"in vec3 TexCoords;\n"
	"\n"
	"uniform samplerCube skybox;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	FragColor = texture(skybox, TexCoords);\n"
	"}\n"
	};

	mShader.createProgram(verShader, fragShader, true);

}

SkyBox::SkyBox()
{
}


SkyBox::~SkyBox()
{
}
