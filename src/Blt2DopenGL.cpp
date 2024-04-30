
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "./mmgr.h"

#define	_X_	0
#define	_Y_	1
#define	_Z_	2

//*****************
//the following functions will blt a 2d texture to the screen.  this
//will be viewed as a sprite.  also need to have trasparency
//included, for the array version the inputs must be arranged in ccw.
//will want to disable the depth buffer before doing sprites
//*****************
void BltGLStyle(RECT *prcSRC, 
				RECT *prcDST, 
				float ImageWidth, 
				float ImageHeight, 
				long ImageId)
{
//	static long curImageId = 0;

	float	UpLeftTexture[2],
			UpRightTexture[2],
			DownLeftTexture[2],
			DownRightTexture[2];

	//only really needed if we are not reading data in correctly
//	if((!ImageWidth) || (!ImageHeight))
//		return;

	//make all texture indicies to be between 0.0 and 1.0
	DownLeftTexture[_X_]	= prcSRC->left	/ ImageWidth;
	DownLeftTexture[_Y_]	= prcSRC->top	/ ImageHeight;

	UpRightTexture[_X_]		= prcSRC->right	/ ImageWidth;
	UpRightTexture[_Y_]		= prcSRC->bottom/ ImageHeight;

	DownRightTexture[_X_]	= UpRightTexture[_X_];
	DownRightTexture[_Y_]	= DownLeftTexture[_Y_];

	UpLeftTexture[_X_]		= DownLeftTexture[_X_];
	UpLeftTexture[_Y_]		= UpRightTexture[_Y_];

	//This is so that we cut down on BindTexture calls
//	if(curImageId != ImageId)
//	{
//		glBindTexture(GL_TEXTURE_2D, ImageId);
//		curImageId = ImageId;
//	}
	glBindTexture(GL_TEXTURE_2D, ImageId);

	glBegin(GL_QUADS);

		glTexCoord2fv(UpLeftTexture);
		glVertex2i(prcDST->left,prcDST->top);

		glTexCoord2fv(DownLeftTexture);
		glVertex2i(prcDST->left,prcDST->bottom);

		glTexCoord2fv(DownRightTexture);
		glVertex2i(prcDST->right,prcDST->bottom);

		glTexCoord2fv(UpRightTexture);
		glVertex2i(prcDST->right,prcDST->top);

	glEnd();

}

void BltGLStyle(
		const int x,
		const int y,
		const RECT	*prcDST,
		float	ImageWidth,
		float	ImageHeight,
		long	ImageId)
{
	float	UpLeftTexture[2],
			UpRightTexture[2],
			DownLeftTexture[2],
			DownRightTexture[2];


	//only really needed if we are not reading data in correctly
//	if((!ImageWidth) || (!ImageHeight))
//		return;

	RECT rcSRC;
	rcSRC.left		= prcDST->left;
	rcSRC.top		= ((int)ImageHeight) - prcDST->bottom;
	rcSRC.right		= prcDST->right;
	rcSRC.bottom	= rcSRC.top + (prcDST->bottom - prcDST->top);

	const int x2 = x + (rcSRC.right	- rcSRC.left);
	const int y2 = y + (rcSRC.bottom- rcSRC.top);

	//
	//make all texture indicies to be between 0.0 and 1.0
	DownLeftTexture[_X_]	=	rcSRC.left	/	ImageWidth;
	DownLeftTexture[_Y_]	=	rcSRC.top	/	ImageHeight;

	UpRightTexture[_X_]		=	rcSRC.right	/	ImageWidth;
	UpRightTexture[_Y_]		=	rcSRC.bottom/	ImageHeight;

	DownRightTexture[_X_]	=	UpRightTexture[_X_];
	DownRightTexture[_Y_]	=	DownLeftTexture[_Y_];

	UpLeftTexture[_X_]		=	DownLeftTexture[_X_];
	UpLeftTexture[_Y_]		=	UpRightTexture[_Y_];

	//
	//do actual 'blt'

	glBindTexture(GL_TEXTURE_2D,ImageId);
	glBegin(GL_QUADS);

		glTexCoord2fv(UpLeftTexture);
		glVertex2i(x, y);

		glTexCoord2fv(DownLeftTexture);
		glVertex2i(x, y2);

		glTexCoord2fv(DownRightTexture);
		glVertex2i(x2, y2);

		glTexCoord2fv(UpRightTexture);
		glVertex2i(x2, y);

	glEnd();

}

void BltGLStyle(
		int x,
		int y,
		int iScaleX,
		int iScaleY,
		const RECT	*prcDST,
		float	ImageWidth,
		float	ImageHeight,
		long	ImageId)
{
	float	UpLeftTexture[2],
			UpRightTexture[2],
			DownLeftTexture[2],
			DownRightTexture[2]; 


	//only really needed if we are not reading data in correctly
//	if((!ImageWidth) || (!ImageHeight))
//		return;

	RECT rcSRC;
	rcSRC.left		= prcDST->left;
	rcSRC.top		= ((int)ImageHeight) - prcDST->bottom;
	rcSRC.right		= prcDST->right;
	rcSRC.bottom	= rcSRC.top + (prcDST->bottom - prcDST->top);

	x -= iScaleX;
	y -= iScaleY;
	const int x2 = x + (rcSRC.right	- rcSRC.left) + iScaleX;
	const int y2 = y + (rcSRC.bottom- rcSRC.top) + iScaleY;

	//
	//make all texture indicies to be between 0.0 and 1.0
	DownLeftTexture[_X_]	=	rcSRC.left	/	ImageWidth;
	DownLeftTexture[_Y_]	=	rcSRC.top	/	ImageHeight;

	UpRightTexture[_X_]		=	rcSRC.right	/	ImageWidth;
	UpRightTexture[_Y_]		=	rcSRC.bottom/	ImageHeight;

	DownRightTexture[_X_]	=	UpRightTexture[_X_];
	DownRightTexture[_Y_]	=	DownLeftTexture[_Y_];

	UpLeftTexture[_X_]		=	DownLeftTexture[_X_];
	UpLeftTexture[_Y_]		=	UpRightTexture[_Y_];

	//
	//do actual 'blt'

	glBindTexture(GL_TEXTURE_2D,ImageId);
	glBegin(GL_QUADS);

		glTexCoord2fv(UpLeftTexture);
		glVertex2i(x, y);

		glTexCoord2fv(DownLeftTexture);
		glVertex2i(x, y2);

		glTexCoord2fv(DownRightTexture);
		glVertex2i(x2, y2);

		glTexCoord2fv(UpRightTexture);
		glVertex2i(x2, y);

	glEnd();

}

/*
void BltGLStyle(
		const float x,
		const float y,
		const float z,
		const RECT	*prcSRC,
		float	ImageWidth,
		float	ImageHeight,
		long	ImageId)
{
	float	UpLeftTexture[2],
			UpRightTexture[2],
			DownLeftTexture[2],
			DownRightTexture[2];

	const int x2 = (long)(x + (prcSRC->right	- prcSRC->left));
	const int y2 = (long)(y + (prcSRC->bottom	- prcSRC->top));

	//
	//make all texture indicies to be between 0.0 and 1.0
	DownLeftTexture[_X_]	=	prcSRC->left	/	ImageWidth;
	DownLeftTexture[_Y_]	=	prcSRC->top		/	ImageHeight;

	UpRightTexture[_X_]		=	prcSRC->right	/	ImageWidth;
	UpRightTexture[_Y_]		=	prcSRC->bottom	/	ImageHeight;

	DownRightTexture[_X_]	=	UpRightTexture[_X_];
	DownRightTexture[_Y_]	=	DownLeftTexture[_Y_];

	UpLeftTexture[_X_]		=	DownLeftTexture[_X_];
	UpLeftTexture[_Y_]		=	UpRightTexture[_Y_];

	//
	//do actual 'blt'

	glBindTexture(GL_TEXTURE_2D,ImageId);
	glBegin(GL_QUADS);

		glTexCoord2fv(UpLeftTexture);
		glVertex3d(x, y, z);

		glTexCoord2fv(DownLeftTexture);
		glVertex3d(x, y2, z);

		glTexCoord2fv(DownRightTexture);
		glVertex3d(x2, y2, z);

		glTexCoord2fv(UpRightTexture);
		glVertex3d(x2, y, z);

	glEnd();

}
*/