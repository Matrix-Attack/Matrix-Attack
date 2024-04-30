#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <stdio.h>

#include <gl/gl.h>

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



extern APP App;
extern GAME_DATA GameData;

void			LoopUntilString(const char *string, FILE *pFile);
unsigned int	ConvertToInt(int numchars, FILE *pFile);
unsigned int	BinaryToInt(int numchars, FILE *pFile);
float			ConvertToDouble(FILE *pFile);


Object3D::Object3D(float x, float y, float z, const P_S8 filename, const P_S8 objname)
{
	ZeroMemory(this, sizeof(Object3D));

	// get the rest of the info from a text file
	ReadObjectFromTextFile(x, y, z, filename, objname);

	// temp
	bDraw		= 1;
	updateFlags	|=	UPDATE_CAM_COORDS;
}

void Object3D::Clear( void )
{
	bDraw			= 0;
	updateFlags		= 0;

	centerPos.Clear();
	dispFromParent.Clear();
	revolution.Clear();
	rotation.Clear();

	numVertices = 0;
	numTexels	= 0;
	numFaces	= 0;
	numChildren = 0;
	
	VertexList	= NULL;
	TexelList	= NULL;
	FaceList	= NULL;
	ChildList	= NULL;
	Parent		= NULL;
}


void Object3D::Destroy( void )
{
	if( VertexList )
	{
		free(VertexList);
		VertexList = NULL;
	}

	if( TexelList )
	{
		free(TexelList);
		TexelList = NULL;
	}

	if( FaceList )
	{
		free(FaceList);
		FaceList = NULL;
	}
	
	if( ChildList )
	{
		// free each child object, then free
		// the child list itself
		for(U32 i = 0; i < numChildren; i++)
		{
			ChildList[i]->Destroy();

			delete ChildList[i];
		}
		free(ChildList);
	}

	Clear();
}

void Object3D::Draw_OpenGL()
{
	
//	Rotate(); //This is Update()'s job -eli
	
//set the material properties to draw in
//	glMaterialfv(GL_FRONT,GL_AMBIENT,matAmbient);
//	glMaterialfv(GL_FRONT,GL_DIFFUSE,matDiffuse);
	//to make it glow
//	glMaterialfv(GL_FRONT,GL_EMISSION,matEmmision);

	//***make it shiny!
//	glMaterialfv(GL_FRONT, GL_SPECULAR, this->glMatSpec);
//	glMaterialfv(GL_FRONT, GL_SHININESS, this->glShiney);
	//***add the ambiant and diffuse colour
//	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, this->glMatAmbDiff);

	for(U32 i = 0; i < numFaces; i++)
	{
		if(FaceList[i].bDraw)
			FaceList[i].Draw_OpenGL();
	}

}

void Object3D::Update_OpenGL( void )
{
	Rotate();
}

void Object3D::Update_DDraw( void )
{
	Rotate();
	GetCameraCoordinates();
	Convert3DTo2D();
	CullFaces();
}

void Object3D::Draw_DDraw()
{
	if(bDraw)
	{
		for(U32 i = 0; i < numFaces; i++)
		{
			if(FaceList[i].bDraw)
				FaceList[i].Draw_DDraw();
		}
	}

	// show the wire frame
//	HDC hdc;
//	HPEN hPen;
//	App.pBackSurface->GetDC(&hdc);
//	hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
//	SelectObject(hdc, hPen);

//	DisplayWireFrame(pApp, hdc);

//	App.pBackSurface->ReleaseDC(hdc);
//	DeleteObject(hPen);
//	}
}





void Object3D::GetCameraCoordinates()
{
	Vertex3D	*pVertex;
	Vector3D	*pCameraPos;
	Camera3D	*pCam	= GameData.pBoard->GetCamera();

	float		c1		= App.Cos[(int)pCam->rotation.x],
				c2		= App.Cos[(int)pCam->rotation.y],
				c3		= App.Cos[(int)pCam->rotation.z],
				s1		= App.Sin[(int)pCam->rotation.x],
				s2		= App.Sin[(int)pCam->rotation.y],
				s3		= App.Sin[(int)pCam->rotation.z],
				e1		= c1 * s3,
				e2		= s1 * c3,
				e3		= c1 * c3,
				e4		= s1 * s3,
				xCamPos	= pCam->worldPos.x,
				yCamPos	= pCam->worldPos.y,
				zCamPos	= pCam->worldPos.z;

	for(U32 i = 0; i < numVertices; i++)
	{
		pVertex			= VertexList + i;
		pCameraPos		= &pVertex->cameraPos;

		// get the position vector from the camera to the vertex
		float		xa	= pVertex->worldCur.x - xCamPos,
					ya	= pVertex->worldCur.y - yCamPos,
					za	= pVertex->worldCur.z - zCamPos,
					e5	= c2 * za;

		pCameraPos->x	= (     c2 * c3) * xa - (     c2 * s3) * ya + (s2 * za);
		pCameraPos->y	= (e1 + e2 * s2) * xa + (e3 - e4 * s2) * ya - (e5 * s1);
		pCameraPos->z	= (e4 - e3 * s2) * xa + (e2 + e1 * c2) * ya + (e5 * c1);

		// temp check if object is behind camera
		if(pCameraPos->z < 1)
		{
			bDraw	= 0;
			return;
		}
	}

	bDraw	= 1;
}


void Object3D::Convert3DTo2D()
{
	// go through each child object and convert to 2D
	for(U32 i = 0; i < numChildren; i++)
	{
		ChildList[i]->Convert3DTo2D();
	}

	// go through each vertex in this object and convert to 2D
	float scale;

	Camera3D	*pCam	= GameData.pBoard->GetCamera();

	for(i = 0; i < numVertices; i++)
	{
		scale	= 640 / VertexList[i].cameraPos.z;
		VertexList[i].pt2D.x = 218 + (int)(VertexList[i].cameraPos.x * scale);
		VertexList[i].pt2D.y = 240 - (int)(VertexList[i].cameraPos.y * scale);
	}
}




void Object3D::CullFaces()
{
	Vertex3D	*pv0, *pv1, *pv2;
	int			x1, y1;

	// this code loops through each of the object's faces and
	// determines whether or not it's visible
	for(U32 i = 0; i < numFaces; i++)
	{
		pv0	= FaceList[i].Vertex[0];
		pv1	= FaceList[i].Vertex[1];
		pv2	= FaceList[i].Vertex[2];
		
		x1	= pv1->pt2D.x;
		y1	= pv1->pt2D.y;

		FaceList[i].bDraw	= ( ((pv0->pt2D.x - x1) * (pv2->pt2D.y - y1)) >
								((pv2->pt2D.x - x1) * (pv0->pt2D.y - y1)) );

		// make sure the face is on the screen
	}
}




void Object3D::DisplayWireFrame(HDC hdc)
{
	// ******************* THIS CODE WILL DISPLAY ALL THE FACES *******************

	for(U32 i = 0; i < numChildren; i++)
	{
		ChildList[i]->DisplayWireFrame(hdc);
	}


	// go through the face list and display each edge
	int			x1, x2, x3, y1, y2, y3;
	Vertex3D	*v1, *v2, *v3;

	for(i = 0; i < numFaces; i++)
	{
		if( (FaceList[i].bDraw) )//&& (FaceList[i].flags == FACE_FLAT_TOP_EDGE) )
		{
			v1 = FaceList[i].Vertex[0];
			v2 = FaceList[i].Vertex[1];
			v3 = FaceList[i].Vertex[2];

			x1 = v1->pt2D.x;
			y1 = v1->pt2D.y;
			x2 = v2->pt2D.x;
			y2 = v2->pt2D.y;
			x3 = v3->pt2D.x;
			y3 = v3->pt2D.y;

			MoveToEx(hdc, x1, y1, NULL);
			LineTo(hdc, x2, y2);
			MoveToEx(hdc, x2, y2, NULL);
			LineTo(hdc, x3, y3);
			MoveToEx(hdc, x3, y3, NULL);
			LineTo(hdc, x1, y1);

			SetPixel(hdc, VertexList[0].pt2D.x, VertexList[0].pt2D.y, RGB(255, 255, 0));

			// temp
			POINT	*p0	= &FaceList[0].Vertex[0]->pt2D,
					*p1	= &FaceList[0].Vertex[1]->pt2D,
					*p2	= &FaceList[0].Vertex[2]->pt2D;

			SetPixel(hdc, p0->x, p0->y, RGB(255, 255, 255));
			SetPixel(hdc, p0->x - 1, p0->y, RGB(255, 255, 255));
			SetPixel(hdc, p0->x + 1, p0->y, RGB(255, 255, 255));
			SetPixel(hdc, p0->x, p0->y - 1, RGB(255, 255, 255));
			SetPixel(hdc, p0->x, p0->y + 1, RGB(255, 255, 255));

			SetPixel(hdc, p1->x, p1->y, RGB(0, 255, 0));
			SetPixel(hdc, p1->x - 1, p1->y, RGB(0, 255, 0));
			SetPixel(hdc, p1->x + 1, p1->y, RGB(0, 255, 0));
			SetPixel(hdc, p1->x, p1->y - 1, RGB(0, 255, 0));
			SetPixel(hdc, p1->x, p1->y + 1, RGB(0, 255, 0));

			SetPixel(hdc, p2->x, p2->y, RGB(0, 128, 255));
			SetPixel(hdc, p2->x - 1, p2->y, RGB(0, 128, 255));
			SetPixel(hdc, p2->x + 1, p2->y, RGB(0, 128, 255));
			SetPixel(hdc, p2->x, p2->y - 1, RGB(0, 128, 255));
			SetPixel(hdc, p2->x, p2->y + 1, RGB(0, 128, 255));
			
			// draw the edges
//			VERTEX ve0, ve1, ve2;

//			ve0.x = x1;
//			ve0.y = y1;
//			ve0.argb = RGBVal(255, 0, 0);

//			ve1.x = x2;
//			ve1.y = y2;
//			ve1.argb = RGBVal(0, 255, 255);

//			ve2.x = x3;
//			ve2.y = y3;
//			ve2.argb = RGBVal(0, 0, 255);

//			triangleShadedFixed(pApp, ve0, ve1, ve2);

		}
	}

	// ******************* THIS CODE WILL JUST DISPLAY THE VERTICES *******************

	// go through the vertex list and display each pixel
//	for(i = 0; i < numVertices; i++)
//	{
//		SetPixel(hdc, VertexList[i].pt2D.x, VertexList[i].pt2D.y, RGB(255, 255, 0));
//	}

	
}



/*
void Object3D::FillFaces()
{
	for(U32 i = 0; i < numFaces; i++)
		FaceList[i].Update();
}
*/




void Object3D::ReadObjectFromTextFile(float x, float y, float z,
									  const P_S8 filename, const P_S8 objname)
{
	// declare a file pointer and open the text file for reading
	FILE	*pFile	= fopen(filename, "r");
	if(!pFile)
		return;

	// now find the object we're gonna load
	LoopUntilString(objname, pFile);


	// now that we've found the object, get its info
	LoopUntilString("> ", pFile);
	numVertices	= ConvertToInt(4, pFile);

	LoopUntilString("> ", pFile);
	numTexels	= ConvertToInt(4, pFile);

	LoopUntilString("> ", pFile);
	numFaces	= ConvertToInt(4, pFile);

	LoopUntilString("> ", pFile);
	numChildren	= ConvertToInt(4, pFile);


	// now that we know the number of vertices and faces,
	// malloc an array for each
	VertexList	= (Vertex3D *)	malloc(numVertices	* sizeof(Vertex3D));
	TexelList	= (POINT *)		malloc(numTexels	* sizeof(POINT));
	FaceList	= (Face3D *)	malloc(numFaces		* sizeof(Face3D));
	ChildList	= (Object3D **)	malloc(numChildren	* sizeof(Object3D *));


	// initialize the vertex list
	Vertex3D	*vCur	= VertexList;

	vCur->cameraPos.x	= vCur->worldCur.x	= vCur->worldDef.x	= x;
	vCur->cameraPos.y	= vCur->worldCur.y	= vCur->worldDef.y	= y;
	vCur->cameraPos.z	= vCur->worldCur.z	= vCur->worldDef.z	= z;

	for(U32 i = 1; i < numVertices; i++)
	{
		vCur	= VertexList + i;

		// get the (x, y, z) position of each vertex
		LoopUntilString(">", pFile);
		vCur->worldCur.x	= vCur->worldDef.x = x + ( vCur->objDef.x = ConvertToDouble(pFile) );

		LoopUntilString(",", pFile);
		vCur->worldCur.y	= vCur->worldDef.y = y + ( vCur->objDef.y = ConvertToDouble(pFile) );

		LoopUntilString(",", pFile);
		vCur->worldCur.z	= vCur->worldDef.z = z + ( vCur->objDef.z = ConvertToDouble(pFile) );
	}


	POINT		*tCur;

	for(i = 0; i < numTexels; i++)
	{
		tCur	= TexelList + i;

		// get the (u, v) position of each texel
		LoopUntilString("> ", pFile);
		tCur->x	= ConvertToInt(4, pFile);

		LoopUntilString(" ", pFile);
		tCur->y	= ConvertToInt(4, pFile);
	}


	// initialize the face list
	Face3D		*fCur;
	for(i = 0; i < numFaces; i++)
	{
		fCur	= FaceList + i;

		// by default, we will draw each face unless it is hidden
		fCur->bDraw		= 1;

		// find out which vertices are contained in each face
		LoopUntilString("> ", pFile);
		fCur->Vertex[0]	= VertexList + ConvertToInt(4, pFile);

		LoopUntilString(" ", pFile);
		fCur->Vertex[1]	= VertexList + ConvertToInt(4, pFile);

		LoopUntilString(" ", pFile);
		fCur->Vertex[2]	= VertexList + ConvertToInt(4, pFile);

		// initialize texels
		LoopUntilString(", ", pFile);
		fCur->ptClip[0]	= TexelList + ConvertToInt(4, pFile);
		
		LoopUntilString(" ", pFile);
		fCur->ptClip[1]	= TexelList + ConvertToInt(4, pFile);

		LoopUntilString(" ", pFile);
		fCur->ptClip[2]	= TexelList + ConvertToInt(4, pFile);

		// initialize pointer to coplanar face
		LoopUntilString(", ", pFile);
		
		int coplanarFaceIndex;

		if( (coplanarFaceIndex	= ConvertToInt(4, pFile) ) != 9999 )
			fCur->coplanarFace	= FaceList + coplanarFaceIndex;
		else
			fCur->coplanarFace	= NULL;
	}


	// close the file when we're done
	fclose(pFile);
}




void Object3D::Rotate()
{
	float		cosA1	= App.Cos[(int)rotation.z],
				sinA1	= App.Sin[(int)rotation.z],
				cosA2	= App.Cos[(int)rotation.y],
				sinA2	= App.Sin[(int)rotation.y],
				cosA3	= App.Cos[(int)rotation.x],
				sinA3	= App.Sin[(int)rotation.x],
				xCenter	= VertexList[0].worldCur.x,
				yCenter	= VertexList[0].worldCur.y,
				zCenter	= VertexList[0].worldCur.z,
				x1, y1, z1, x2, y2, z2;


	Vertex3D	*vCur;

	for(U32 i = 1; i < numVertices; i++)
	{
		vCur	= VertexList + i;
		x1		= vCur->worldDef.x - xCenter;
		y1		= vCur->worldDef.y - yCenter;
		z1		= vCur->worldDef.z - zCenter;

		x2		= (x1 * cosA1) - (y1 * sinA1);
		y2		= (x1 * sinA1) + (y1 * cosA1);
		z2		= (x2 * sinA2) + (z1 * cosA2);

		vCur->worldCur.x	= xCenter + (x2 * cosA2) - (z1 * sinA2);
		vCur->worldCur.y	= yCenter + (z2 * sinA3) + (y2 * cosA3);
		vCur->worldCur.z	= zCenter + (z2 * cosA3) - (y2 * sinA3);
	}
}



/*
void Object3D::HandleKeyInput()
{
	if(App.bQ)
	{
		if( (rotation.x += 4) > 359)
			rotation.x -= 360;
	}
	if(App.bW)
	{
		if( (rotation.x -= 4) < 0)
			rotation.x += 360;
	}
	if(App.bA)
	{
		if( (rotation.y += 4) > 359)
			rotation.y -= 360;
	}
	if(App.bS)
	{
		if( (rotation.y -= 4) < 0)
			rotation.y += 360;
	}
	if(App.bZ)
	{
		if( (rotation.z += 4) > 359)
			rotation.z -= 360;
	}
	if(App.bX)
	{
		if( (rotation.z -= 4) < 0)
			rotation.z += 360;
	}
}

*/



// this function reads <numchars> characters from the file pointer and
// converts it to an integer
//unsigned int Shape3D::ConvertToInt(int numchars, FILE *pFile)
unsigned int ConvertToInt(int numchars, FILE *pFile)
{
	char	*tempstring	= (char *)malloc(numchars * sizeof(char));
	int		result		= 0;
	int		multiplier	= 1;

	// read <numchars> characters...
	for(int i = 0; i < numchars; i++)
		tempstring[i] = getc(pFile);

	// ...and convert those characters to an integer
	for(i = numchars - 1; i >= 0; i--)
	{
		result		+= (tempstring[i] - '0') * multiplier;
		multiplier	*= 10;
	}

	free(tempstring);

	return result;
}





// this function reads <numchars> characters from the file pointer and
// converts it to an integer
//unsigned int Shape3D::BinaryToInt(int numchars, FILE *pFile)
unsigned int BinaryToInt(int numchars, FILE *pFile)
{
	char	*tempstring	= (char *)malloc(numchars * sizeof(char));
	int		result		= 0;
	int		multiplier	= 1;

	// read <numchars> characters...
	for(int i = 0; i < numchars; i++)
		tempstring[i] = getc(pFile);

	// ...and convert those characters to an integer
	for(i = numchars - 1; i >= 0; i--)
	{
		result		+= (tempstring[i] - '0') * multiplier;
		multiplier	<<= 1;
	}

	free(tempstring);

	return result;
}




//float Shape3D::ConvertToDouble(FILE *pFile)
float ConvertToDouble(FILE *pFile)
{
	float wholePart;
	float decimalPart;

	// check for negative numbers
	if(getc(pFile) == '-')
	{
		wholePart	=  -(float)ConvertToInt(4, pFile);
		getc(pFile);
		decimalPart	= (-(float)ConvertToInt(4, pFile)) / 10000;
	}
	else
	{
		wholePart	=  (float)ConvertToInt(4, pFile);
		getc(pFile);
		decimalPart	= ((float)ConvertToInt(4, pFile)) / 10000;
	}

	return (wholePart + decimalPart);
}