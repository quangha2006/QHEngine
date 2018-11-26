#include "QHTexture.h"
#include "Timer.h"
#include "TGA.h"

namespace QHTexture
{
	bool TextureFromFile(const char * path, const std::string & directory, unsigned int textureID)
	{
		std::string fullpath = directory + '/' + std::string(path);
		uint64_t time_begin = Timer::getMillisecond();

		if (strlen(fullpath.c_str()) > 3)
		{
			std::string filetype = fullpath.substr(fullpath.length() - 3, 3);
			if (!filetype.compare("tga") || !filetype.compare("TGA"))
			{

				TGA::TextureData texture;
				if (LoadTGA(texture, fullpath.c_str()))
				{
					glBindTexture(GL_TEXTURE_2D, textureID);
					// Set the filtering mode
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTexImage2D(GL_TEXTURE_2D, 0, texture.type, texture.width, texture.height, 0, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
					glGenerateMipmap(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, 0);
					CheckGLError("glTexImage2D");
				}
				else
				{
					LOGE("ERROR when load: %s \n", fullpath.c_str());
					return false;
				}
			}
			else
			{
				uint64_t time_begin = Timer::getMillisecond();
				if (SOIL_load_OGL_texture(fullpath.c_str(), SOIL_LOAD_RGBA, textureID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS) == 0)
				{
					LOGE("ERROR when load: %s \n", fullpath.c_str());
					return false;
				}
			}
		}

		uint64_t time_end = Timer::getMillisecond();
		LOGI("ENDLOAD TGA  Load: %dms %s\n", (int)(time_end - time_begin), fullpath.c_str());
		return true;
	}
}