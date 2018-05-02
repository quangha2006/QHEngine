#include "SkyBox.h"
#include <SOIL.h>
#include "Utils.h"

void SkyBox::Init(const char * texturepath)
{
	const char * verShader = {
	"#version 100\n"
	"attribute vec3 aPos;\n"
	"varying vec3 TexCoords;\n"
	"\n"
	"uniform mat4 projection;\n"
	"uniform mat4 view;\n"
	"uniform mat4 model;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	TexCoords = aPos;\n"
	"	vec4 temp = projection * view * model * vec4(aPos, 1.0);\n"
	"	temp.w = temp.z + 0.01; // add small offset for preventing z-buffer fighting\n"
	"	gl_Position = temp;//projection * view * model * vec4(aPos, 1.0);\n"
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

	textureID = loadCubemap(texturepath, faces);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), &skyboxVertices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), &skyboxIndices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SkyBox::Draw(Camera *camera)
{
	mShader.use();
	glDepthMask(GL_FALSE);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	if (mShader.getPosAttribute() >= 0)
	{
		glEnableVertexAttribArray(mShader.getPosAttribute());
		glVertexAttribPointer(mShader.getPosAttribute(), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	mShader.setInt("skybox", 0);
	mShader.setMat4("view", camera->view);
	mShader.setMat4("projection", camera->projection);
	mShader.setMat4("model", model);

	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, (void*)0);
	glDepthMask(GL_TRUE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SkyBox::setScale(float scale)
{
	this->scale = scale;
	this->model = glm::scale(model, glm::vec3(scale));
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
	skyboxVertices = new GLfloat[24] {
	-1.0,  1.0,  1.0,
	-1.0, -1.0,  1.0,
	 1.0, -1.0,  1.0,
	 1.0,  1.0,  1.0,
	-1.0,  1.0, -1.0,
	-1.0, -1.0, -1.0,
	 1.0, -1.0, -1.0,
	 1.0,  1.0, -1.0
	};
	skyboxIndices = new GLuint[24] {
		0, 1, 2, 3,
		3, 2, 6, 7,
		7, 6, 5, 4,
		4, 5, 1, 0,
		0, 3, 7, 4,
		1, 2, 6, 5
	};
	model = glm::mat4(1.0f);
	scale = 50.0f;
	setScale(scale);
}


SkyBox::~SkyBox()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &textureID);
	delete[] skyboxVertices;
	delete[] skyboxIndices;
}
