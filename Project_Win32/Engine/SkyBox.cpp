#include "SkyBox.h"
#include <SOIL.h>
#include "Utils.h"

void SkyBox::Init(const char * texturepath)
{
	const char * verShader = {
	"#version 100\n"
	"attribute vec3 aPos;\n"
	"attribute vec3 aTexCoords;\n"
	"varying vec3 TexCoords;\n"
	"\n"
	"uniform mat4 projection;\n"
	"uniform mat4 view;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	TexCoords = aTexCoords;\n"
	"	gl_Position = projection * view * vec4(aPos, 1.0);\n"
	"}\n"
	};
	const char *fragShader = {
	"precision highp float;\n"
	"\n"
	"varying vec3 TexCoords;\n"
	"\n"
	"uniform samplerCube skybox;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = texture(skybox, TexCoords);\n"
	"}\n"
	};

	mShader.LoadShader(verShader, fragShader, true);

	std::vector<std::string> faces
	{
		"Right.jpg",
		"Left.jpg",
		"Top.jpg",
		"Bottom.jpg",
		"Front.jpg",
		"Back.jpg"
	};

	cubemapTexture = loadCubemap(texturepath, faces);


	glGenBuffers(1, &VBO);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 108 * 2 * sizeof(float), &skyboxVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SkyBox::Draw(Camera *camera)
{
	mShader.use();
	glDepthMask(GL_FALSE);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if (mShader.getPosAttribute() >= 0)
	{
		glEnableVertexAttribArray(mShader.getPosAttribute());
		glVertexAttribPointer(mShader.getPosAttribute(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	}

	if (mShader.getTexCoodAttribute() >= 0)
	{
		glEnableVertexAttribArray(mShader.getTexCoodAttribute());
		glVertexAttribPointer(mShader.getTexCoodAttribute(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	mShader.setInt("skybox", 0);
	mShader.setMat4("view", camera->view);
	mShader.setMat4("projection", camera->projection);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

unsigned int SkyBox::loadCubemap(const char * texturepath, std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		std::string fullPath = Utils::getResourcesFolder();
		fullPath += texturepath;
		fullPath += '/';
		fullPath += faces[i];

		unsigned char *data = SOIL_load_image(fullPath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			SOIL_free_image_data(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << fullPath << std::endl;
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
	int x = 40;
	skyboxVertices = new float[108 * 2] {
		// positions          
		-1.0f * x, 1.0f * x, -1.0f * x,		-1.0f, 1.0f, -1.0f,
		-1.0f * x, -1.0f * x, -1.0f * x,	-1.0f, -1.0f, -1.0f,
		1.0f * x, -1.0f * x, -1.0f * x,		1.0f, -1.0f, -1.0f,
		1.0f * x, -1.0f * x, -1.0f * x,		1.0f, -1.0f, -1.0f,
		1.0f * x, 1.0f * x, -1.0f * x,		1.0f, 1.0f, -1.0f,
		-1.0f * x, 1.0f * x, -1.0f * x,		-1.0f, 1.0f, -1.0f,

		-1.0f * x, -1.0f * x, 1.0f * x,		-1.0f, -1.0f, 1.0f,
		-1.0f * x, -1.0f * x, -1.0f * x,	-1.0f, -1.0f, -1.0f,
		-1.0f * x, 1.0f * x, -1.0f * x,		-1.0f, 1.0f, -1.0f,
		-1.0f * x, 1.0f * x, -1.0f * x,		-1.0f, 1.0f, -1.0f,
		-1.0f * x, 1.0f * x, 1.0f * x,		-1.0f, 1.0f, 1.0f,
		-1.0f * x, -1.0f * x, 1.0f * x,		-1.0f, -1.0f, 1.0f,

		1.0f * x, -1.0f * x, -1.0f * x,		1.0f, -1.0f, -1.0f,
		1.0f * x, -1.0f * x, 1.0f * x,		1.0f, -1.0f, 1.0f,
		1.0f * x, 1.0f * x, 1.0f * x,		1.0f, 1.0f, 1.0f,
		1.0f * x, 1.0f * x, 1.0f * x,		1.0f, 1.0f, 1.0f,
		1.0f * x, 1.0f * x, -1.0f * x,		1.0f, 1.0f, -1.0f,
		1.0f * x, -1.0f * x, -1.0f * x,		1.0f, -1.0f, -1.0f,

		-1.0f * x, -1.0f * x, 1.0f * x,		-1.0f, -1.0f, 1.0f,
		-1.0f * x, 1.0f * x, 1.0f * x,		-1.0f, 1.0f, 1.0f,
		1.0f * x, 1.0f * x, 1.0f * x,		1.0f, 1.0f, 1.0f,
		1.0f * x, 1.0f * x, 1.0f * x,		1.0f, 1.0f, 1.0f,
		1.0f * x, -1.0f * x, 1.0f * x,		1.0f, -1.0f, 1.0f,
		-1.0f * x, -1.0f * x, 1.0f * x,		-1.0f, -1.0f, 1.0f,

		-1.0f * x, 1.0f * x, -1.0f * x,		-1.0f, 1.0f, -1.0f,
		1.0f * x, 1.0f * x, -1.0f * x,		1.0f, 1.0f, -1.0f,
		1.0f * x, 1.0f * x, 1.0f * x,		1.0f, 1.0f, 1.0f,
		1.0f * x, 1.0f * x, 1.0f * x,		1.0f, 1.0f, 1.0f,
		-1.0f * x, 1.0f * x, 1.0f * x,		-1.0f, 1.0f, 1.0f,
		-1.0f * x, 1.0f * x, -1.0f * x,		-1.0f, 1.0f, -1.0f,

		-1.0f * x, -1.0f * x, -1.0f * x,	-1.0f, -1.0f, -1.0f,
		-1.0f * x, -1.0f * x, 1.0f * x,		-1.0f, -1.0f, 1.0f,
		1.0f * x, -1.0f * x, -1.0f * x,		1.0f, -1.0f, -1.0f,
		1.0f * x, -1.0f * x, -1.0f * x,		1.0f, -1.0f, -1.0f,
		-1.0f * x, -1.0f * x, 1.0f * x,		-1.0f, -1.0f, 1.0f,
		1.0f * x, -1.0f * x, 1.0f * x,		1.0f, -1.0f, 1.0f
	};
}


SkyBox::~SkyBox()
{
	glDeleteBuffers(1, &VBO);
	delete[] skyboxVertices;
}
