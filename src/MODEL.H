#ifndef model_h
#define model_h

typedef char			S8,	 *P_S8;
typedef unsigned short	U16, *P_U16;
typedef unsigned int	U32, *P_U32;



// definitions for flags used by the Face3D struct
#define FACE_POINTS_LEFT		0x1		// if this flag is set, the triangle points to the left
#define FACE_POINTS_RIGHT		0x2		// triangle points to the right
#define FACE_FLAT_TOP_EDGE		0x4		// the triangle's top edge is horizontal
#define FACE_FLAT_BOTTOM_EDGE	0x8		// the triangle's bottom edge is horizontal


typedef class tagAPI_DDraw		API_DDRAW,	*P_API_DDRAW;


struct Face3D
{
	Face3D();
	int			bDraw;			// determines whether we draw this particular face or not

	int			type;			// describes the type of triangle for filling purposes

	U16			baseColor;
	U16			color;
	
	RECT		boundRect;
	
	Vector3D	unitNormal;		// the unit normal vector for the face

	Vertex3D	*Vertex[3];		// an array of 3 pointers to vertices forming a triangle.

	POINT		*ptClip[3];		// an array of clipping coordinates used for texture mapping

	Face3D		*coplanarFace;	// pointer to any coplanar faces (if any)

	// member functions
	void	GetUnitNormal();
	void	SortVertices();
	void	DetermineType();
	void	GetBoundingRect();
	void	GetFillColor();
	void	Fill(P_API_DDRAW pDDraw);

	// temp
	void	SetColor(unsigned short RGBvalue);
	void	SetVertices(int x0, int y0, int x1, int y1, int x2, int y2);
	void	Draw_DDraw();
	void	Draw_OpenGL();
private:
	inline void Rasterize(P_API_DDRAW pDDraw,	int top, int bottom,
							float dxStart, float dxStop, float &xStart, float &xStop);
};




#define UPDATE_ROTATION					0x1
#define UPDATE_CAM_COORDS				0x2

struct Object3D
{
	int			bDraw,
				updateFlags;

	Vector3D	centerPos,		// center position of the object (in world coordinates)
				dispFromParent,	// displacement from the parent (used for revolutions)
				revolution,		// angles of revolution about the parent
				rotation;		// the object's angles of rotation about the x-, y-, and z-axes

	U32			numVertices;	// number of vertices composing the object

	U32			numTexels;		// number of texel clipping coordinates (u, v)

	U32			numFaces;		// number of faces composing the object

	U32			numChildren;	// number of child objects controlled by this object

	Vertex3D	*VertexList;	// a malloc'ed array of vertices composing the object

	POINT		*TexelList;		// list of all the texel clipping coordinates (u, v)

	Face3D		*FaceList;		// a malloc'ed array of triangles composing the object

	Object3D	**ChildList;	// a malloc'ed array of child objects controlled by this object

	Object3D	*Parent;		// pointer to this object's parent object (NULL if no parent exists)


public:
	Object3D	( void ) { Clear();	  }
	~Object3D	( void ) { Destroy(); }
	Object3D	( float x, float y, float z, const P_S8 filename, const P_S8 objname);
	
	void	Clear	( void );
	void	Destroy	( void );

	// member functions
	inline U32	GetNumVertices()
				{ return numVertices; }

	inline U32	GetNumFaces()
				{ return numFaces; }

	void	GetCameraCoordinates();
	void	Convert3DTo2D();
	void	CullFaces();
	void	DisplayWireFrame(HDC hdc);
//	void	FillFaces();
	void	ReadObjectFromTextFile(	float x, float y, float z,
									const P_S8 filename, const P_S8 objname);
	void	Rotate();
	void	HandleKeyInput();

//	void	TextureMap( P_SURFDESC pSurfDesc);

	void	Update_DDraw();
	void	Update_OpenGL();

	void	Draw_DDraw();
	void	Draw_OpenGL();
};




#endif