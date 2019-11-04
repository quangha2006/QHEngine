#include "SkyBox.h"
#include "Utils.h"
#include <stb_image.h>
#include "QHTexture.h"
#include "RenderManager.h"

void SkyBox::Init(const char * texturepath)
{
	const char * verShader = {
	"layout (location = 0) in vec3 aPos;\n"
	"out vec3 TexCoords;\n"
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
	"precision highp float;\n"
	"\n"
	"in vec3 TexCoords;\n"
	"\n"
	"uniform samplerCube skybox;\n"
	"uniform bool GammaCorrection;\n"
	"layout(location = 0) out vec4 FragColor;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	vec4 color = texture(skybox, TexCoords);\n"
	"if (GammaCorrection == true)\n"
	"{\n"
	"	color.rgb = pow(color.rgb, vec3(2.2));\n"
	"}\n"
	"	FragColor = color;\n"
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
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_initialized = true;
}

void SkyBox::Draw(Camera *camera)
{
	if (!m_initialized) 
		return;

	mShader.use();

	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(mVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	mShader.setInt("skybox", 0);
	glm::mat4 projection = camera->GetProjection();
	glm::mat4 view = glm::mat4(glm::mat3(camera->GetView())); // remove translation from the view matrix

	mShader.setMat4("WorldViewProjectionMatrix", projection * view);
	mShader.setInt("GammaCorrection",RenderManager::getInstance()->isEnableGammaCorrection());
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
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
	: m_initialized(false)
	, mProjection(glm::mat4(1.0f))
{
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
}


SkyBox::~SkyBox()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteTextures(1, &textureID);
	delete[] skyboxVertices;
}
