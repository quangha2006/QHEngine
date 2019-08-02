#include "QHTexture.h"
#include "Timer.h"
#include "TGA.h"
#include "Utils.h"
#include "Logs.h"
//#include <SOIL.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace QHTexture
{
	//bool TextureFromFile(const char * path, const std::string & directory, unsigned int textureID)
	//{
	//	std::string fullpath = directory + '/' + std::string(path);
	//	uint64_t time_begin = Timer::getMillisecond();

	//	if (strlen(fullpath.c_str()) > 3)
	//	{
	//		std::string filetype = fullpath.substr(fullpath.length() - 3, 3);
	//		if (!filetype.compare("tga") || !filetype.compare("TGA"))
	//		{

	//			TGA::TextureData texture;
	//			if (LoadTGA(texture, fullpath.c_str()))
	//			{
	//				glBindTexture(GL_TEXTURE_2D, textureID);
	//				// Set the filtering mode
	//				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	//				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//				glTexImage2D(GL_TEXTURE_2D, 0, texture.type, texture.width, texture.height, 0, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
	//				glGenerateMipmap(GL_TEXTURE_2D);
	//				glBindTexture(GL_TEXTURE_2D, 0);
	//				CheckGLError("glTexImage2D");
	//			}
	//			else
	//			{
	//				LOGE("Load texture ERROR!: %s \n", fullpath.c_str());
	//				return false;
	//			}
	//		}
	//		else
	//		{
	//			//uint64_t time_begin = Timer::getMillisecond();
	//			if (SOIL_load_OGL_texture(fullpath.c_str(), SOIL_LOAD_RGBA, textureID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS) == 0)
	//			{
	//				LOGE("Load texture ERROR!: %s \n", fullpath.c_str());
	//				return false;
	//			}
	//		}
	//	}

	//	uint64_t time_end = Timer::getMillisecond();
	//	LOGI("Load texture: %3dms %s\n", (int)(time_end - time_begin), fullpath.c_str());
	//	return true;
	//}
	GLuint GenTextureId()
	{
		GLuint textureID = 0;
		glGenTextures(1, &textureID);
		CheckGLError("GenTextureId");
		return textureID;
	}
	GLuint TextureFromFile(const char * path, const std::string & directory, GLint &width, GLint &height, GLuint textureID, bool gammaCorrection)
	{
		std::string fullpath = directory + '/' + std::string(path);
		uint64_t time_begin = Timer::getMillisecond();
		GLuint texid = textureID;
		if (texid == 0)
			texid = GenTextureId();

		GLint nrComponents;
		unsigned char *data = stbi_load(fullpath.c_str(), &width, &height, &nrComponents, 0);
		//unsigned char *data = SOIL_load_image(fullpath.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum internalFormat;
			GLenum dataFormat;
			if (nrComponents == 1)
			{
				internalFormat = dataFormat = GL_RED;
			}
			else if (nrComponents == 3)
			{
				internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
				dataFormat = GL_RGB;
			}
			else if (nrComponents == 4)
			{
				internalFormat = gammaCorrection ? 0x8C42 : GL_RGBA;
				//internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
				//internalFormat = GL_RGBA;//gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
				dataFormat = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, texid);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			LOGE("Texture failed to load at path: %s", path);
			stbi_image_free(data);
			return 0;
		}

		uint64_t time_end = Timer::getMillisecond();
		LOGI("Load texture: %3dms %s\n", (int)(time_end - time_begin), fullpath.c_str());
		return texid;
	}
	GLuint loadCubemap(const char * texturepath, std::vector<std::string> faces, bool gammaCorrection)
	{
		GLuint textureID = 0;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrComponents;
		std::string resourcesFolder = Utils::getResourcesFolder();
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			std::string fullPath = resourcesFolder;
			fullPath += texturepath;
			fullPath += '/';
			fullPath += faces[i];
			GLenum internalFormat;
			GLenum dataFormat;

			unsigned char *data = stbi_load(fullPath.c_str(), &width, &height, &nrComponents, 0);
			//unsigned char *data = SOIL_load_image(fullPath.c_str(), &width, &height, &nrComponents, 0);
			if (data)
			{
				if (nrComponents == 1)
				{
					internalFormat = dataFormat = GL_RED;
				}
				else if (nrComponents == 3)
				{
					internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
					dataFormat = GL_RGB;
				}
				else if (nrComponents == 4)
				{
					//internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
					internalFormat = gammaCorrection ? 0x8C42 : GL_RGBA;
					//internalFormat = GL_RGBA;
					dataFormat = GL_RGBA;
				}
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
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
}