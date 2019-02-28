#include "SkyBox.h"
//#include <SOIL.h>
#include "Utils.h"
#include <ShaderManager.h>
#include <stb_image.h>
#include "QHTexture.h"

void SkyBox::Init(const char * texturepath)
{
	const char * verShader = {
	"#version 100\n"
	"attribute vec3 aPos;\n"
	"varying vec3 TexCoords;\n"
	"\n"
	"uniform mat4 WorldViewProjectionMatrix;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	TexCoords = aPos;\n"
	"	vec4 temp = WorldViewProjectionMatrix * vec4(aPos, 1.0);\n"
	"	temp.w = temp.z + 0.01; // add small offset for preventing z-buffer fighting\n"
	"	gl_Position = temp.xyww;\n"
	"}\n"
	};
	const char *fragShader = {
	"#version 100\n"
	"precision highp float;\n"
	"\n"
	"varying vec3 TexCoords;\n"
	"\n"
	"uniform samplerCube skybox;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = textureCube(skybox, TexCoords);\n"
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

	textureID = QHTexture::loadCubemap(texturepath, faces, false);

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), &skyboxVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_initialized = true;
}

void SkyBox::Draw(Camera *camera)
{
	if (!m_initialized) return;
	GLint currentid = ShaderManager::getInstance()->GetCurrentProgram();
	mShader.use();
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	if (mShader.getPosAttribute() >= 0)
	{
		glEnableVertexAttribArray(mShader.getPosAttribute());
		glVertexAttribPointer(mShader.getPosAttribute(), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	mShader.setInt("skybox", 0);

	glm::mat4 lookat_tmp = camera->WorldViewProjectionMatrix * model;

	mShader.setMat4("WorldViewProjectionMatrix", lookat_tmp);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (currentid != -1)
		ShaderManager::getInstance()->setUseProgram(currentid);
}

void SkyBox::setScale(float scale)
{
	this->scale = scale;
	this->model = glm::scale(model, glm::vec3(scale));
}

GLuint SkyBox::loadCubemap(const char * texturepath, std::vector<std::string> faces)
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

		unsigned char *data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << fullPath << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

GLuint SkyBox::getTextureID()
{
	return textureID;
}

SkyBox::SkyBox()
{
	m_initialized = false;
	skyboxVertices = new GLfloat[108]{
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	model = glm::mat4(1.0f);
	scale = 900.0f;
	setScale(scale);
}


SkyBox::~SkyBox()
{
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1, &textureID);
	delete[] skyboxVertices;
}
