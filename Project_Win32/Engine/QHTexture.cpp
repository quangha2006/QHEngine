#include "QHTexture.h"
#include "Timer.h"
#include "TGA.h"
#include "Utils.h"
#include "Logs.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace QHTexture
{
	
	GLuint GenTextureId()
	{
		GLuint textureID = 0;
		glGenTextures(1, &textureID);
		CheckGLError("GenTextureId");
		return textureID;
	}
	void DeleteTexture(Texture & tex)
	{
		glDeleteTextures(1, &tex.id);
	}

	std::string ReplacePathETC(std::string currentPath, int mipmap)
	{
		std::string expand = Utils::toString("_mip_%d.pkm", mipmap);
		int index = currentPath.find_last_of('/');
		currentPath.insert(index + 1, "ETC2/");
		index = currentPath.find_last_of('.');
		if (index > -1)
			currentPath.replace(index, currentPath.length() - 1, expand);
		else
			currentPath += expand;
		return currentPath;
	}
	unsigned char *loadPKMData(const char *filename, int &width, int &height, GLsizei &imgsize, GLenum &internalFormat, GLenum &dataFormat)
	{
		// PKM file consists of a header with information about image (stored in 16 first bits) and image data.
		const int      sizeOfETCHeader = 16;// sizeof(ETC2Header);
		unsigned char* tempHeader = NULL;
		unsigned char* tempTextureData = NULL;
		unsigned int   fileSize = 0;
		int   dataSize = 0;

		FILE *file = fopen(filename, "rb");
		if (file == NULL)
		{
			return NULL;
		}

		fseek(file, 0, SEEK_END);
		fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);
		//LOGI("FileSize: %d\n", fileSize);
		dataSize = fileSize - sizeOfETCHeader;
		tempHeader = new unsigned char[sizeOfETCHeader];
		tempTextureData = new unsigned char[dataSize];

		size_t read = fread(tempHeader, sizeof(unsigned char), sizeOfETCHeader, file);
		//LOGI("File Header Size: %d\n", read);
		if (read != sizeOfETCHeader) LOGE("Failed to read header PKM file: %s\n", filename);
		
		read = fread(tempTextureData, sizeof(unsigned char), dataSize, file);
		//LOGI("File Data Size: %d\n", read);
		if (read != dataSize) LOGE("Failed to read data PKM file: %s\n", filename);
		fclose(file);

		ETC2Header tempEtcHeader(tempHeader);
		width = ((tempEtcHeader.widthMSB << 8) | tempEtcHeader.widthLSB);
		height = ((tempEtcHeader.heightMSB << 8) | tempEtcHeader.heightLSB);
		imgsize = [](unsigned char paddedWidthMSB, unsigned char paddedWidthLSB, unsigned char paddedHeightMSB, unsigned char paddedHeightLSB)-> GLsizei {
			unsigned short a = (paddedWidthMSB << 8) | paddedWidthLSB;
			unsigned short b = (paddedHeightMSB << 8) | paddedHeightLSB;
			return (GLsizei)(a * b);
		} (tempEtcHeader.paddedWidthMSB, tempEtcHeader.paddedWidthLSB, tempEtcHeader.paddedHeightMSB, tempEtcHeader.paddedHeightLSB);

		if (read < imgsize)
			LOGE("Failed to read data PKM file: Read = %u, imgsize = %d", read, imgsize);

		internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC;
		dataFormat = GL_RGBA;
		delete[] tempHeader;
		return tempTextureData;
	}
	
	Texture TextureFromFile(const std::string& path, const std::string & directory, aiTextureType type, GLuint textureID, bool gammaCorrection)
	{
		std::string fullpath = directory + '/' + path;
		uint64_t time_begin = Timer::getMillisecond();

		GLint nrComponents = 0;
		GLenum internalFormat = GL_NONE;
		GLenum dataFormat = GL_NONE;
		GLint width = 0;
		GLint height = 0;
		GLsizei imgsize = 0;
		unsigned char *data = NULL;
		bool ETC2FORMAT = false;
		bool tryLoadAgain = true;
		static bool isSupportETC2(true);//Utils::IsExtensionSupported("ETC");
		if (isSupportETC2)
		{
			std::string ETCPath;
			std::string PrevLoad;
			int mipmap = 0;
			do {
				ETCPath = ReplacePathETC(fullpath, mipmap);
				data = loadPKMData(ETCPath.c_str(), width, height, imgsize, internalFormat, dataFormat);
				if (data)
				{
					if (textureID == 0 && mipmap == 0)
						textureID = GenTextureId();
					if (mipmap == 0)
						glBindTexture(GL_TEXTURE_2D, textureID);
					glCompressedTexImage2D(GL_TEXTURE_2D, mipmap, internalFormat, width, height, 0, imgsize, data);
					PrevLoad = ETCPath;
					CheckGLError("Gen ETC2 Texture");
					delete[] data;
					mipmap++;
				}
				else
					break;
			} while (mipmap <= 7); // Num Mipmap

			if (mipmap > 0)
			{
				uint64_t time_end = Timer::getMillisecond();
				LOGI("Load ETC2 texture: %3ums, total mipmap = %2d, %s\n", (unsigned int)(time_end - time_begin), mipmap, PrevLoad.c_str());
				tryLoadAgain = false;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmap - 1);
			}
				
		}
		if (tryLoadAgain)
		{
			data = stbi_load(fullpath.c_str(), &width, &height, &nrComponents, 0);
			if (!data)
				LOGE("Failed to open: %s\n", path);
			else
			{
				switch (nrComponents)
				{
				case 1: internalFormat = dataFormat = GL_RED;
					break;
				case 3: internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
					dataFormat = GL_RGB;
					break;
				case 4: internalFormat = gammaCorrection ? 0x8C42 : GL_RGBA;
					//internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
					//internalFormat = GL_RGBA;//gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
					dataFormat = GL_RGBA;
					break;
				}
				if (textureID == 0)
					textureID = GenTextureId();
				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				stbi_image_free(data);
				uint64_t time_end = Timer::getMillisecond();
				LOGI("Load texture: %3ums, %s\n", (unsigned int)(time_end - time_begin), fullpath.c_str());
			}

		}
		if (textureID != 0)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		Texture texture;
		texture.id = textureID;
		texture.path = path;
		texture.width = width;
		texture.height = height;
		texture.type = type;
		
		return texture;
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