#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <gl/gl.h>
#include "./mmgr.h"

#include "tga.h"


// Error Codes
#define TGA_FILE_NOT_FOUND          13 // file was not found
#define TGA_BAD_IMAGE_TYPE          14 // color mapped image or image is not uncompressed
#define TGA_BAD_DIMENSION			15 // dimension is not a power of 2 
#define TGA_BAD_BITS				16 // image bits is not 8, 24 or 32 
#define TGA_BAD_DATA				17 // image data could not be loaded 

GLenum texFormat;

//checkImageSize  Make sure its a power of 2.
int checkSize (int x)
{
    if (x == 2	 || x == 4 || 
        x == 8	 || x == 16 || 
        x == 32  || x == 64 ||
        x == 128 || x == 256 || x == 512)
        return 1;
    else return 0;
}

//Reads in RGBA data for a 32bit image. 
unsigned char *getRGBA (FILE *s, int size)
{
    unsigned char *rgba;
    unsigned char temp;
    int bread;
    int i;
	unsigned char transparentColour[3];

    rgba = (unsigned char *)malloc (size * 4); 

    if (rgba == NULL)
        return 0;

    bread = fread (rgba, sizeof (unsigned char), size * 4, s); 

    // TGA is stored in BGRA, make it RGBA 
    if (bread != size * 4)
    {
        free (rgba);
        return 0;
    }

	//
	//set the transparent colour to be the pixel in the bottom left of the image
	transparentColour[0]=	rgba[0];
	transparentColour[1]=	rgba[1];
	transparentColour[2]=	rgba[2];

    for (i = 0; i < size * 4; i += 4 )
    {
		//
		//set transparency by setting alpha to 0 or 1
		((transparentColour[0] == rgba[i])	&&
		(transparentColour[1] == rgba[i+1])	&&
		(transparentColour[2] == rgba[i+2]))?
			rgba[i + 3]=	0:	rgba[i + 3]=	255;

        temp = rgba[i];
        rgba[i] = rgba[i + 2];
        rgba[i + 2] = temp;
    }

    texFormat = GL_RGBA;
    return rgba;
}

//Reads in RGB data for a 24bit image. 
unsigned char *getRGB (FILE *s, int size)
{
    unsigned char *rgb;
    unsigned char temp;
    int bread;
    int i;

    rgb = (unsigned char *)malloc (size * 3); 

    if (rgb == NULL)
        return 0;

    bread = fread (rgb, sizeof (unsigned char), size * 3, s);

    if (bread != size * 3)
    {
        free (rgb);
        return 0;
    }

    // TGA is stored in BGR, make it RGB 
    for (i = 0; i < size * 3; i += 3)
    {
        temp = rgb[i];
        rgb[i] = rgb[i + 2];
        rgb[i + 2] = temp;
    }

    texFormat = GL_RGB;

    return rgb;
}

//Gets the grayscale image data.  Used as an alpha channel.
unsigned char *getGray (FILE *s, int size)
{
    unsigned char *grayData;
    int bread;

    grayData = (unsigned char *)malloc (size);

    if (grayData == NULL)
        return 0;

    bread = fread (grayData, sizeof (unsigned char), size, s);

    if (bread != size)
    {
        free (grayData);
        return 0;
    }

    texFormat = GL_ALPHA;

    return grayData;
}

//Gets the image data for the specified bit depth.
char *getData (FILE *s, int sz, int iBits)
{
    if (iBits == 32)
        return (char *)getRGBA (s, sz);
    else if (iBits == 24)
        return (char *)getRGB (s, sz);	
    else if (iBits == 8)
        return (char *)getGray (s, sz);
	else
		return NULL;
}

//Called when there is an error loading the .tga file.
int returnError (FILE *s, int error)
{
    fclose (s);
    return error;
}

//Loads up a targa file.  Supported types are 8,24 and 32 uncompressed images.  
//id is the texture ID to bind too.
// NOTE - If you get missing TGA file messages but everything appears normal,
// the files may be read-only which causes problems.
int LoadGLTarga (char *name, int id)
{
    unsigned char type[4];
    unsigned char info[7];
    unsigned char *imageData = NULL;
    int imageWidth, imageHeight;
    int imageBits, size;
    FILE *s;

    if (!(s = fopen (name, "rb")))
        return TGA_FILE_NOT_FOUND;

    fread (&type, sizeof (char), 3, s); // read in colormap info and image type, byte 0 ignored
    fseek (s, 12, SEEK_SET);			// seek past the header and useless info
    fread (&info, sizeof (char), 6, s);

    if (type[1] != 0 || (type[2] != 2 && type[2] != 3))
        returnError (s, TGA_BAD_IMAGE_TYPE);

    imageWidth = info[0] + info[1] * 256; 
    imageHeight = info[2] + info[3] * 256;
    imageBits =	info[4]; 

    size = imageWidth * imageHeight; 

    // make sure dimension is a power of 2 
    if (!checkSize (imageWidth) || !checkSize (imageHeight))
        returnError (s, TGA_BAD_DIMENSION);

    // make sure we are loading a supported type 
    if (imageBits != 32 && imageBits != 24 && imageBits != 8)
        returnError (s, TGA_BAD_BITS);

    imageData = (unsigned char *)getData (s, size, imageBits);

    // no image data */
    if (imageData == NULL)
        returnError (s, TGA_BAD_DATA);

    fclose (s);

    glBindTexture (GL_TEXTURE_2D, id);	//id == texture name or id.

    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		// 
//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Magnification
//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Minification
    
	//This function is what makes sure that

	//NOTE: GL_REPLACE is needed rather than GL_MODULATE because we don't want  
	// our textures flushed with whatever last vertex color was set.. argh..
	// i spent some time on this one.
	//if( id <= 10)
	//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//else
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D (GL_TEXTURE_2D, 0, texFormat, imageWidth,
		imageHeight, 0, texFormat, GL_UNSIGNED_BYTE, imageData);

    // release data, its been uploaded 
    free (imageData);

    return 0;
}