#include "stdafx.h"
#include "TGA.h"
#include <stdlib.h>     /* malloc, free, rand */
#include "SOIL.h"
#include "Logs.h"
namespace TGA
{
	GLubyte uTGAcompare[12] = { 0,0, 2,0,0,0,0,0,0,0,0,0 };
	// Compressed TGA Header
	GLubyte cTGAcompare[12] = { 0,0,10,0,0,0,0,0,0,0,0,0 };

	unsigned int CreateSimpleTexture2D(const char* path, unsigned int textureId = 0)
	{
		if (textureId != 0)
		{
			// Use tightly packed data
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			// Generate a texture object
			glGenTextures(1, &textureId);
		}
		// Load the texture
		TextureData texture;
		LoadTGA(texture, path);
		
		// Bind the texture object
		glBindTexture(GL_TEXTURE_2D, textureId);
		// Set the filtering mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (texture.imageData)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, texture.type, texture.width, texture.height, 0, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
		}
		CheckGLError("glTexImage2D");

		glBindTexture(GL_TEXTURE_2D, 0);
		if (texture.imageData)
			free(texture.imageData);
		return textureId;
	}

	void FlipVertical(TextureData & texture)
	{
		if (texture.imageData)
		{
			GLuint bytesPerPixel = texture.Bpp / 8;
			GLuint datasize = bytesPerPixel * texture.width * texture.height;
			GLuint bytesPerRow = bytesPerPixel * texture.width;
			GLubyte * buffer = (GLubyte *)malloc(datasize);
			for (int i = 0; i < texture.height; i++)
			{
				memcpy(&buffer[(texture.height - i - 1) * bytesPerRow], &(texture.imageData[i * bytesPerRow]), bytesPerRow);
			}
			free(texture.imageData);
			texture.imageData = buffer;
		}
	}


	bool LoadTGA(TextureData &texture, const char * filename)
	{

		FILE * fTGA;                    // Declare File Pointer
		TGAHeader tgaheader;     // Used To Store Our File Header

		fTGA = fopen(filename, "rb");           // Open File For Reading
		if (fTGA == NULL)                // If Here Was An Error
		{
			LOGE("No such file: %s\n", filename);
			return false;
		}
		// Attempt To Read The File Header
		if (fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)
		{
			LOGE("Error while read tga header\n");
			fclose(fTGA);
			return false;
		}
		// If The File Header Matches The Uncompressed Header
		if (memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
		{
			// Load An Uncompressed TGA
			if (LoadUncompressedTGA(texture, filename, fTGA))
			{
				FlipVertical(texture);
				return true;
			}
			else
				return false;
		}
		// If The File Header Matches The Compressed Header
		else if (memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
		{
			// Load A Compressed TGA
			if (LoadCompressedTGA(texture, filename, fTGA))
			{
				FlipVertical(texture);
				return true;
			}
			else
				return false;
		}

		// If It Doesn't Match Either One
		LOGE("tga file doesn't match Uncompressed or Compressed tga\n");
		fclose(fTGA);
		return false;
	}

	bool LoadUncompressedTGA(TextureData &texture, const char * filename, FILE * fTGA)
	{
		TGAHeader tgaheader;     // Used To Store Our File Header
		Image tga;                 // Used To Store File Information
		//fprintf(stderr, "Load Uncompressed TGA: %s\n", filename);
		// Attempt To Read Next 6 Bytes
		int header = fread(tga.header, sizeof(tga.header), 1, fTGA);
		if (header == 0)
		{
			LOGE("Error: Load UncompressedTGA\n");
			return false;               // Return False
		}
		texture.width = tga.header[1] * 256 + tga.header[0];   // Calculate Height
		texture.height = tga.header[3] * 256 + tga.header[2];   // Calculate The Width
		texture.Bpp = tga.header[4];            // Calculate Bits Per Pixel
		tga.Width = texture.width;              // Copy Width Into Local Structure
		tga.Height = texture.height;            // Copy Height Into Local Structure
		tga.Bpp = texture.Bpp;                  // Copy Bpp Into Local Structure
												 // Make Sure All Information Is Valid
		if ((texture.width <= 0) || (texture.height <= 0) || ((texture.Bpp != 24) && (texture.Bpp != 32)))
		{
			LOGE("Error: Get width, height, bpp\n");
			return false;               // Return False
		}
		if (texture.Bpp == 24)               // Is It A 24bpp Image?
			texture.type = GL_RGB;       // If So, Set Type To GL_RGB
		else                        // If It's Not 24, It Must Be 32
			texture.type = GL_RGBA;      // So Set The Type To GL_RGBA
		tga.bytesPerPixel = (tga.Bpp / 8);      // Calculate The BYTES Per Pixel
												// Calculate Memory Needed To Store Image
		tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);
		// Allocate Memory
		texture.imageData = (GLubyte *)malloc(tga.imageSize);
		if (texture.imageData == NULL)           // Make Sure It Was Allocated Ok
		{
			LOGE("Error: Allocat texture->imageData\n");
			return false;               // If Not, Return False
		}
		// Attempt To Read All The Image Data
		// Flip Image
		/*GLuint bytesPerRow = tga.bytesPerPixel * tga.Width;
		GLubyte * buffer = (GLubyte *)malloc(bytesPerRow);
		for (int i = 0; i < tga.Height; i++)
		{
			if (fread(buffer, 1, bytesPerRow, fTGA) == bytesPerRow)
			{
				memcpy(&texture.imageData[(tga.Height - i - 1) * bytesPerRow], buffer, bytesPerRow);
			}
			else
			{
				fprintf(stderr, "Error: Read All The Image Data\n");
				free(buffer);
				free(texture.imageData);
				return false;               // If We Cant, Return False
			}
		}
		free(buffer);
		*/
		if (fread(texture.imageData, 1, tga.imageSize, fTGA) != tga.imageSize)
		{
			LOGE("Error: Read All The Image Data\n");
			return false;               // If We Cant, Return False
		}

		// Start The Loop
		// Change from BGR to RGB so OpenGL can read the image data.
		for (GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
		{
			// 1st Byte XOR 3rd Byte XOR 1st Byte XOR 3rd Byte
			texture.imageData[cswap] ^= texture.imageData[cswap + 2] ^=
				texture.imageData[cswap] ^= texture.imageData[cswap + 2];
		}

		fclose(fTGA);                   // Close The File
		return true;                    // Return Success
	}

	bool LoadCompressedTGA(TextureData &texture, const char * filename, FILE * fTGA) // Load COMPRESSED TGAs
	{
		TGAHeader tgaheader;     // Used To Store Our File Header
		Image tga;                 // Used To Store File Information
		//fprintf(stderr, "Load Compressed TGA: %s\n", filename);
		if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)					// Attempt to read header
		{
			LOGE("Could not read info header");		// Display Error
			if (fTGA != NULL)													// If file is open
			{
				fclose(fTGA);													// Close it
			}
			return false;														// Return failed
		}

		texture.width = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
		texture.height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
		texture.Bpp = tga.header[4];										// Determine Bits Per Pixel
		tga.Width = texture.width;										// Copy width to local structure
		tga.Height = texture.height;										// Copy width to local structure
		tga.Bpp = texture.Bpp;											// Copy width to local structure

		if ((texture.width <= 0) || (texture.height <= 0) || ((texture.Bpp != 24) && (texture.Bpp != 32)))	//Make sure all texture info is ok
		{
			LOGE("Invalid texture information");	// If it isnt...Display error
			if (fTGA != NULL)													// Check if file is open
			{
				fclose(fTGA);													// Ifit is, close it
			}
			return false;														// Return failed
		}

		if (texture.Bpp == 24)												// If the BPP of the image is 24...
			texture.type = GL_RGB;											// Set Image type to GL_RGB
		else																// Else if its 32 BPP
			texture.type = GL_RGBA;											// Set image type to GL_RGBA

		tga.bytesPerPixel = (tga.Bpp / 8);									// Compute BYTES per pixel
		tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute amout of memory needed to store image
		texture.imageData = (GLubyte *)malloc(tga.imageSize);					// Allocate that much memory

		if (texture.imageData == NULL)											// If it wasnt allocated correctly..
		{
			LOGE("Could not allocate memory for image");	// Display Error
			fclose(fTGA);														// Close file
			return false;														// Return failed
		}

		GLuint pixelcount = tga.Height * tga.Width;							// Nuber of pixels in the image
		GLuint currentpixel = 0;												// Current pixel being read
		GLuint currentbyte = 0;												// Current byte 
		GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);			// Storage for 1 pixel

		do
		{
			GLubyte chunkheader = 0;											// Storage for "chunk" header

			if (fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0)				// Read in the 1 byte header
			{
				LOGE("Could not read RLE header");	// Display Error
				if (fTGA != NULL)												// If file is open
				{
					fclose(fTGA);												// Close file
				}
				if (texture.imageData != NULL)									// If there is stored image data
				{
					free(texture.imageData);									// Delete image data
				}
				return false;													// Return failed
			}

			if (chunkheader < 128)												// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
			{																	// that follow the header
				chunkheader++;													// add 1 to get number of following color values
				for (short counter = 0; counter < chunkheader; counter++)		// Read RAW color values
				{
					if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) // Try to read 1 pixel
					{
						LOGE("Could not read image data");		// IF we cant, display an error

						if (fTGA != NULL)													// See if file is open
						{
							fclose(fTGA);													// If so, close file
						}

						if (colorbuffer != NULL)												// See if colorbuffer has data in it
						{
							free(colorbuffer);												// If so, delete it
						}

						if (texture.imageData != NULL)										// See if there is stored Image data
						{
							free(texture.imageData);										// If so, delete it too
						}

						return false;														// Return failed
					}

					// write to memory
					texture.imageData[currentbyte] = colorbuffer[2];				    // Flip R and B vcolor values around in the process 
					texture.imageData[currentbyte + 1] = colorbuffer[1];
					texture.imageData[currentbyte + 2] = colorbuffer[0];

					if (tga.bytesPerPixel == 4)												// if its a 32 bpp image
					{
						texture.imageData[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
					}

					currentbyte += tga.bytesPerPixel;										// Increase thecurrent byte by the number of bytes per pixel
					currentpixel++;															// Increase current pixel by 1

					if (currentpixel > pixelcount)											// Make sure we havent read too many pixels
					{
						LOGE("Too many pixels read");			// if there is too many... Display an error!

						if (fTGA != NULL)													// If there is a file open
						{
							fclose(fTGA);													// Close file
						}

						if (colorbuffer != NULL)												// If there is data in colorbuffer
						{
							free(colorbuffer);												// Delete it
						}

						if (texture.imageData != NULL)										// If there is Image data
						{
							free(texture.imageData);										// delete it
						}

						return false;														// Return failed
					}
				}
			}
			else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
			{
				chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
				if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)		// Attempt to read following color values
				{
					LOGE("Could not read from file");			// If attempt fails.. Display error (again)

					if (fTGA != NULL)														// If thereis a file open
					{
						fclose(fTGA);														// Close it
					}

					if (colorbuffer != NULL)													// If there is data in the colorbuffer
					{
						free(colorbuffer);													// delete it
					}

					if (texture.imageData != NULL)											// If thereis image data
					{
						free(texture.imageData);											// delete it
					}

					return false;															// return failed
				}

				for (short counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated 
				{																			// by the header
					texture.imageData[currentbyte] = colorbuffer[2];					// switch R and B bytes areound while copying
					texture.imageData[currentbyte + 1] = colorbuffer[1];
					texture.imageData[currentbyte + 2] = colorbuffer[0];

					if (tga.bytesPerPixel == 4)												// If TGA images is 32 bpp
					{
						texture.imageData[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
					}

					currentbyte += tga.bytesPerPixel;										// Increase current byte by the number of bytes per pixel
					currentpixel++;															// Increase pixel count by 1

					if (currentpixel > pixelcount)											// Make sure we havent written too many pixels
					{
						LOGE("Too many pixels read");			// if there is too many... Display an error!

						if (fTGA != NULL)													// If there is a file open
						{
							fclose(fTGA);													// Close file
						}

						if (colorbuffer != NULL)												// If there is data in colorbuffer
						{
							free(colorbuffer);												// Delete it
						}

						if (texture.imageData != NULL)										// If there is Image data
						{
							free(texture.imageData);										// delete it
						}

						return false;														// Return failed
					}
				}
			}
		} while (currentpixel < pixelcount);													// Loop while there are still pixels left
		fclose(fTGA);																		// Close the file
		return true;																		// return success
	}
}