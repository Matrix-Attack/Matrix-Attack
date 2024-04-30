#include <math.h>
#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>

#include <gl\gl.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "vector.h"
#include "camera3d.h"
#include "model.h"
#include "gem3d.h"
#include "player.h"
#include "board.h"

#include "defines.h"



extern APP App;
extern GAME_DATA GameData;

void __fastcall Memset16Bit ( P_U16 dst, U16 fill, int num );

int DoRectsIntersect(POINT &pt0, POINT &pt1, POINT &pt2, POINT &pt3)
{
	if( (	INRANGE(pt0.x, pt2.x, pt3.x) || INRANGE(pt1.x, pt2.x, pt3.x) ||
			INRANGE(pt2.x, pt0.x, pt1.x) || INRANGE(pt3.x, pt0.x, pt1.x) ) &&
		(	INRANGE(pt0.y, pt2.y, pt3.y) || INRANGE(pt1.y, pt2.y, pt3.y) ||
			INRANGE(pt2.y, pt0.y, pt1.y) || INRANGE(pt3.y, pt0.y, pt1.y) ) )
		return 1;

	return 0;
}


void SwapPointers(Vertex3D **pt0, Vertex3D **pt1)
{
	Vertex3D	*ptTemp	= *pt0;
	*pt0				= *pt1;
	*pt1				= ptTemp;
}
void SwapPointers2(POINT **pt0, POINT **pt1)
{
	POINT		*ptTemp	= *pt0;
	*pt0				= *pt1;
	*pt1				= ptTemp;
}

// Constructor for the face class
Face3D::Face3D()
{
	bDraw = 0;		

	type = 0;			

	baseColor = 0;
	color = 0;
	
	SetRect(&boundRect, 0, 0, 0, 0);
	
	unitNormal.x = 0;
	unitNormal.y = 0;
	unitNormal.z = 0;

	Vertex[0]  = NULL;	
	Vertex[1]  = NULL;	
	Vertex[2]  = NULL;	

	ptClip[0] = NULL;
	ptClip[1] = NULL;
	ptClip[2] = NULL;

	coplanarFace = NULL;	
}


// this function calculates the unit vector which is tangent
// to the face. this vector can be used for lighting effects
// by determining the angle at which light is striking it.
void Face3D::GetUnitNormal()
{
	Vector3D	*vertex0	= &Vertex[0]->worldCur,
				*vertex1	= &Vertex[1]->worldCur,
				*vertex2	= &Vertex[2]->worldCur;

	Vector3D	vectorA, vectorB;

	// take the cross product of vectors running along
	// two adjacent sides of the face. then, scale it
	// down to unit size to get our unit normal
	vectorA.x	= vertex2->x - vertex1->x;
	vectorA.y	= vertex2->y - vertex1->y;
	vectorA.z	= vertex2->z - vertex1->z;

	vectorB.x	= vertex0->x - vertex1->x;
	vectorB.y	= vertex0->y - vertex1->y;
	vectorB.z	= vertex0->z - vertex1->z;

	unitNormal	= ~(vectorA % vectorB);
}




void Face3D::GetBoundingRect()
{
	POINT	*pt0	= &Vertex[0]->pt2D,
			*pt1	= &Vertex[1]->pt2D,
			*pt2	= &Vertex[2]->pt2D;

	boundRect.left		= MIN3(pt0->x, pt1->x, pt2->x);
	boundRect.top		= MIN3(pt0->y, pt1->y, pt2->y);
	boundRect.right		= MAX3(pt0->x, pt1->x, pt2->x);
	boundRect.bottom	= MAX3(pt0->y, pt1->y, pt2->y);
}




// sorts the vertices by swapping them counter-clockwise
// until pt0 is on top. in the case of triangles with a
// flat top, pt0 is moved to the top left side.
void Face3D::SortVertices()
{
	POINT	*pt0	= &Vertex[0]->pt2D,
			*pt1	= &Vertex[1]->pt2D,
			*pt2	= &Vertex[2]->pt2D;

	while( (pt0->y > pt1->y) || (pt0->y > pt2->y) )
	{
		// swap the points counter-clockwise
		SwapPointers(Vertex + 0, Vertex + 1);
		SwapPointers(Vertex + 1, Vertex + 2);
		SwapPointers2(&pt0, &pt1);
		SwapPointers2(&pt1, &pt2);
	}

	// if triangle has a flat top, make sure pt0 is
	// moved to the top left side
	if(pt0->y == pt1->y)
	{
		// swap the points counter-clockwise
		SwapPointers(Vertex + 0, Vertex + 1);
		SwapPointers(Vertex + 1, Vertex + 2);
	}
}




// determines what kind of triangle we have (flat top, flat bottom,
// etc...) it is assumed that the vertices have already been sorted.
void Face3D::DetermineType()
{
	POINT	*pt0	= &Vertex[0]->pt2D,
			*pt1	= &Vertex[1]->pt2D,
			*pt2	= &Vertex[2]->pt2D;

	if(pt0->y == pt2->y)
		type = FACE_FLAT_TOP_EDGE;

	else if(pt1->y == pt2->y)
		type = FACE_FLAT_BOTTOM_EDGE;

	else if(pt1->y < pt2->y)
		type = FACE_POINTS_LEFT;

	else
		type = FACE_POINTS_RIGHT;
}




// this function draws the scanlines of a triangle from <top> to
// <bottom>. it doesn't look very pretty, but it saves a *lot* of
// code in the Fill() function, which we're more likely to look at.
//
// note: this is a private function, so you guys'll never have to call
// this directly.
inline void Face3D::Rasterize(P_API_DDRAW pDDraw, int top, int bottom, 
							  float dxStart, float dxStop, float &xStart, float &xStop)
{
	P_U16	pScreen		= pDDraw->Screen.pScreen;		// pointer to the screen surface
	int		realWidth	= pDDraw->Screen.iRealWidth,	// width of the surface (in memory)
			x,										// x-coord of first pixel in the scanline
			y,										// the current scanline
			triangleWidth;							// width of the triangle at the current scanline


	// draw each scanline (if it's on the screen)
	for(y = top; (y < bottom) && (y < 400); y++)
	{
		// check for clipping along the screen's top edge
		if(y >= 80)
		{
			// check for clipping along the screen's left edge
			if( (	x = (int)floor(xStart) ) < 135 )
					x = 135;

			// check for clipping along the screen's right edge
			// and set the span width
			triangleWidth = xStop > 302 ? 302 - x : (int)ceil(xStop) - x;

			// use a memory fill to color the entire span
			Memset16Bit(pScreen + y * realWidth + x, color, triangleWidth);
		}

		// calculate the beginning and ending x-coords for the next scanline
		xStart	+= dxStart;
		xStop	+= dxStop;
	}
}




// fills a triangle with solid color. the triangle's width
// is computed at each scanline, and an assembly mem filler
// is used to paint the entire width.
//
// it is assumed that the vertices have already been sorted.
void Face3D::Fill(P_API_DDRAW pDDraw)
{
	POINT	*pt0	= &Vertex[0]->pt2D,
			*pt1	= &Vertex[1]->pt2D,
			*pt2	= &Vertex[2]->pt2D;

	int				realWidth	= pDDraw->Screen.iRealWidth;
	unsigned short	*pScreen	= pDDraw->Screen.pScreen;

	float	xStart,			// the beginning x-coord for the current scanline
			xStop,			// the ending x-coord for the current scanline
			dxStart,		// amount that the starting x-coord changes each scanline
			dxStop;			// amount that the ending x-coord changes each scanline


	// draw the triangle
	switch(type)
	{
	case FACE_FLAT_TOP_EDGE:
	case FACE_FLAT_BOTTOM_EDGE:
		xStart		= (float)pt0->x;
		dxStart		= (pt1->x - pt0->x) / (float)(pt1->y - pt0->y);

		if(type == FACE_FLAT_TOP_EDGE)
		{
			xStop	= (float)pt2->x;
			dxStop	= (pt1->x - pt2->x) / (float)(pt1->y - pt2->y);
		}
		else
		{
			xStop	= (float)pt0->x;
			dxStop	= (pt2->x - pt0->x) / (float)(pt2->y - pt0->y);
		}

		Rasterize(pDDraw, pt0->y, pt1->y, dxStart, dxStop, xStart, xStop);

		break;

	case FACE_POINTS_LEFT:
	case FACE_POINTS_RIGHT:
		xStart		= (float)pt0->x;
		xStop		= (float)pt0->x;

		dxStart		= (pt1->x - pt0->x) / (float)(pt1->y - pt0->y);
		dxStop		= (pt2->x - pt0->x) / (float)(pt2->y - pt0->y);


		if(type == FACE_POINTS_LEFT)
		{
			Rasterize(pDDraw, pt0->y, pt1->y, dxStart, dxStop, xStart, xStop);
			dxStart	= (pt2->x - pt1->x) / (float)(pt2->y - pt1->y);
			Rasterize(pDDraw, pt1->y, pt2->y, dxStart, dxStop, xStart, xStop);
		}
		else
		{
			Rasterize(pDDraw, pt0->y, pt2->y, dxStart, dxStop, xStart, xStop);
			dxStop	= (pt1->x - pt2->x) / (float)(pt1->y - pt2->y);
			Rasterize(pDDraw, pt2->y, pt1->y, dxStart, dxStop, xStart, xStop);
		}

		break;
	}
}




void Face3D::SetColor(unsigned short RGBvalue)
{
	color	= RGBvalue;
}




void Face3D::SetVertices(int x0, int y0, int x1, int y1, int x2, int y2)
{
	POINT	*pt0	= &Vertex[0]->pt2D,
			*pt1	= &Vertex[1]->pt2D,
			*pt2	= &Vertex[2]->pt2D;

	pt0->x	= x0;
	pt0->y	= y0;

	pt1->x	= x1;
	pt1->y	= y1;

	pt2->x	= x2;
	pt2->y	= y2;
}





void Face3D::GetFillColor()
{
/*	Vector3D	*pt0			= &Vertex[0]->cameraPos,
				*pt1			= &Vertex[1]->cameraPos,
				*pt2			= &Vertex[2]->cameraPos,
				cameraToFace	= *pt0 + *pt1 + *pt2;

	Camera3D	*pCam			= GameData.pBoard->GetCamera();

	float	distance			= !cameraToFace,
			cosAngle			= LIMIT(unitNormal * pCam->headlight, 0, 1),
			intensity			= cosAngle * LIMIT(640 / distance, 0, 1);

	U16		red					= (U16)(((baseColor & 0xF800) >> 11) * intensity),
			green				= (U16)(((baseColor & 0x7E0) >> 5) * intensity),
			blue				= (U16)(((baseColor & 0x1F)) * intensity);

	color	= (red << 11) | (green << 5) | (blue);
*/
	color	= baseColor;
}




void Face3D::Draw_DDraw()
{
	GetBoundingRect();

	POINT	pt0	= {boundRect.left, boundRect.top},
			pt1	= {boundRect.right, boundRect.bottom},
			pt2	= {135, 80},
//			pt3	= {639, 479};
			pt3	= {302, 401};

	if( bDraw = DoRectsIntersect(pt0, pt1, pt2, pt3) )
	{
		SortVertices();

		DetermineType();

		if(coplanarFace && coplanarFace->bDraw)
			color	= coplanarFace->color;
		else
		{
			GetUnitNormal();
			GetFillColor();
		}

		Fill(App.pDDraw);
	}
}

void Face3D::Draw_OpenGL()
{
	float x[3], y[3], z[3];

	x[0]	= Vertex[0]->worldCur.x;
	y[0]	= Vertex[0]->worldCur.y;
	z[0]	= Vertex[0]->worldCur.z - 640; //gl's z and ddraw's z are 640 apart

	x[1]	= Vertex[1]->worldCur.x;
	y[1]	= Vertex[1]->worldCur.y;
	z[1]	= Vertex[1]->worldCur.z - 640;

	x[2]	= Vertex[2]->worldCur.x;
	y[2]	= Vertex[2]->worldCur.y;
	z[2]	= Vertex[2]->worldCur.z - 640;

//	U16 Convert15() {return (U16) (((r&0xF8)<<7) + ((g&0xF8)<<2) + (b>>3)); }
//	U16 Convert16() {return (U16) (((r&0xF8)<<8) + ((g&0xFC)<<3) + (b>>3)); }
//	U32 Convert32() {return (U32) ( (r<<16) + (g<<8) + b); }

	//Convert our petty 16-bit baseColor into awesome 24-bit openGL color
	glColor3ub(((baseColor & 0xF800)>>11)*8,
			   ((baseColor & 0x7E0)>>5)*4,
				(baseColor & 0x1F)*8);
	
	glBegin(GL_TRIANGLES);
	(coplanarFace)?
		glNormal3d(coplanarFace->unitNormal.x,
				   coplanarFace->unitNormal.y,
				   coplanarFace->unitNormal.z):
		glNormal3d(unitNormal.x,
				   unitNormal.y,
				   unitNormal.z);

		glVertex3d(x[0], y[0], z[0]);
		
		glVertex3d(x[1], y[1], z[1]);
		
		glVertex3d(x[2], y[2], z[2]);
	
	glEnd();

}