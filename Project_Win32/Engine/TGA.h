#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <cstdint>

#define TRUE 1
#define FALSE 0

namespace TGA
{
	struct TextureData
	{
		GLubyte* imageData;      // Hold All The Color Values For The Image.
		GLuint  Bpp;             // Hold The Number Of Bits Per Pixel.          
		GLuint width;            // The Width Of The Entire Image.  
		GLuint height;           // The Height Of The Entire Image. 
		GLuint textureId;            // Texture ID For Use With glBindTexture.  
		GLuint type;             // Data Stored In * ImageData (GL_RGB Or GL_RGBA)
		TextureData()
		{
			imageData = NULL;
		}
		~TextureData()
		{
			if (imageData != NULL)
				free(imageData);
		}
	};

	typedef struct
	{
		GLubyte Header[12];      // File Header To Determine File Type
	} TGAHeader;

	typedef struct
	{
		GLubyte header[6];      // Holds The First 6 Useful Bytes Of The File
		GLuint bytesPerPixel;   // Number Of BYTES Per Pixel (3 Or 4)
		GLuint imageSize;       // Amount Of Memory Needed To Hold The Image
		GLuint type;            // The Type Of Image, GL_RGB Or GL_RGBA
		GLuint Height;          // Height Of Image                 
		GLuint Width;           // Width Of Image              
		GLuint Bpp;             // Number Of BITS Per Pixel (24 Or 32)
	} Image;
	/*======================================================================*/

	bool LoadTGA(TextureData &texture, const char * filename);
		// Load An Uncompressed File
	bool LoadUncompressedTGA(TextureData &texture, const char *, FILE *);
		// Load A Compressed File
	bool LoadCompressedTGA(TextureData &texture, const char *, FILE *);

	GLuint CreateSimpleTexture2D(const char* path, unsigned int textureId);

	void FlipVertical(TextureData &texture);
}