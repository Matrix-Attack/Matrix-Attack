/****************
tim berry and eli emmerson
creation date:10-27-99
****************/
#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>

#include "./mmgr.h"
#include "Bitmap.h"

//rgba indicies
#define	_RED_	0
#define	_GREEN_	1
#define	_BLUE_	2
#define	_ALPHA_	3

//error codes
#define	NO_BITMAP_FOOL					1
#define	NOT_HEIGHT_WIDTH_OF_GOODNESS	2

//checkImageSize  Make sure its a power of 2.
int checkSize (int x);

/*******************
this is all eli's
good code'n.  this
will hook up a texture
into gl space.
*******************/
void LoadGLTexture(
	unsigned char	*pPixelDataRGBA,
	long			IdBind,
	GLenum			TextureFormat,
	long			Width,
	long			Height)
{
	//
	//time to do some gl stuff
	glBindTexture(
		GL_TEXTURE_2D,
		IdBind);

	glPixelStorei(
		GL_UNPACK_ALIGNMENT,
		1);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S,
		GL_REPEAT);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T,
		GL_REPEAT);

	//magnification
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);//GL_LINEAR);

	//minification
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);

	glTexEnvf(
		GL_TEXTURE_ENV,
		GL_TEXTURE_ENV_MODE,
		GL_REPLACE);

	glTexImage2D(
		GL_TEXTURE_2D,		//it's a 2d texture
		0,
		TextureFormat,		//format of texture, rgb, rgba etc
		Width,				//width of image
		Height,				//height of image
		0,
		TextureFormat,		//format of texture, rgb, rgba etc
		GL_UNSIGNED_BYTE,	//stored in unsigned bytes
		pPixelDataRGBA);	//data of image, stored rgba
}

/******************
this will load a
bitmap into a openGL
texture
******************/
long LoadGLBitmap(
	char			FileName[],
	long			IdBind,
	unsigned long	Transparency,
	unsigned char	AlphaValue)
{
	long				height,
						width,
						offsetAligned,
						offsetRGBA,
						bytesWide;

	long				counterA,
						counterB;

	BITMAPINFO			BitmapInfo;
	BITMAPFILEHEADER	HeaderInfo;

	FILE				*fp;

	unsigned char		*pPixelDataAligned,
						*pPixelDataRGBA;

	unsigned char		rgb[3],
						rgbTransparent[3];

	GLenum				textureFormat=GL_RGBA;

	//
	//open the bitmap for binary reading
	fp=fopen(FileName,"rb");

	//if no bitmap present
	if(!fp)
		return NO_BITMAP_FOOL;

	//
	//read in the bitmap header data
	fread(&HeaderInfo,sizeof(BITMAPFILEHEADER),1,fp);

	//
	//read the bitmap info in
	fread(&BitmapInfo,sizeof(BITMAPINFO),1,fp);

	//
	//set the height and width
	width=	BitmapInfo.bmiHeader.biWidth;
	height=	BitmapInfo.bmiHeader.biHeight;

	bytesWide= (((width*3)+3) & ~3);

	if(!(checkSize(width) && checkSize(height)))
		return NOT_HEIGHT_WIDTH_OF_GOODNESS;

	//
	//get to the bit offset
	fseek(fp,HeaderInfo.bfOffBits,SEEK_SET);

	//
	//allocate array of pixel data in byte aligned format
	pPixelDataAligned=	(unsigned char*)malloc(bytesWide*height);

	//
	//read in pixel data, still byte aligned
	fread(pPixelDataAligned,sizeof(unsigned char),bytesWide*height,fp);

	//
	//now allocate the rgba array.  height * width * 4(r,g,b,a)
	pPixelDataRGBA=		(unsigned char*)malloc(height*width*4);

	//
	//set the transparency colour
	rgbTransparent[_RED_]=		pPixelDataAligned[2];
	rgbTransparent[_GREEN_]=	pPixelDataAligned[1];
	rgbTransparent[_BLUE_]=		pPixelDataAligned[0];

	//
	//store the rgba values into the pPixelDataRGBA from pPixelDataAligned
	for(counterA=0;counterA < height;++counterA)
	{
		for(counterB=0;counterB < width;++counterB)
		{
			//
			//calculate offset into the aligned array
			offsetAligned=	(counterA*bytesWide) + (counterB*3);

			rgb[_RED_]=		pPixelDataAligned[offsetAligned + 2];
			rgb[_GREEN_]=	pPixelDataAligned[offsetAligned + 1];
			rgb[_BLUE_]=	pPixelDataAligned[offsetAligned + 0];

			//
			//calculate offset into the rgba array
			offsetRGBA=		(counterA*width*4) + (counterB*4);

			pPixelDataRGBA[offsetRGBA + _RED_]=		rgb[_RED_];
			pPixelDataRGBA[offsetRGBA + _GREEN_]=	rgb[_GREEN_];
			pPixelDataRGBA[offsetRGBA + _BLUE_]=	rgb[_BLUE_];

			(Transparency)?
				//check if transparent, if so set the colour
				((rgbTransparent[_RED_]			== rgb[_RED_]	) &&
					(rgbTransparent[_GREEN_]	== rgb[_GREEN_]	) &&
					(rgbTransparent[_BLUE_]		== rgb[_BLUE_]	))?
					pPixelDataRGBA[offsetRGBA + _ALPHA_]=	0:
					pPixelDataRGBA[offsetRGBA + _ALPHA_]=	AlphaValue:
				pPixelDataRGBA[offsetRGBA + _ALPHA_]=	AlphaValue;
		}
	}

	LoadGLTexture(
		pPixelDataRGBA,
		IdBind,
		textureFormat,
		width,
		height);

	//
	//cloe the file and free some data
	fclose(fp);
	free(pPixelDataAligned);
	free(pPixelDataRGBA);

	return 0;
}
