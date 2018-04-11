#include "SkyBox.h"
#include <SOIL.h>


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

	std::vector<std::string> faces
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};

	cubemapTexture = loadCubemap(faces);

}

unsigned int SkyBox::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			SOIL_free_image_data(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			SOIL_free_image_data(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

SkyBox::SkyBox()
{
}


SkyBox::~SkyBox()
{
}
