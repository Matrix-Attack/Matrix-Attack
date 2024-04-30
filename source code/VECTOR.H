#ifndef vector_h
#define vector_h







struct Vector3D
{
	float x, y, z;
	
	inline void		Clear		( void )	{ x = y = z = 0.0f; }
	// member functions
	inline float	GetLength	( void );		// returns the vector's length
	inline float	Dot			( Vector3D &b);	// returns the dot product with b
	inline Vector3D	Cross		( Vector3D &b);	// returns the cross product with b

	inline float	GetCos		( Vector3D &b);	// returns cos(angle) with b
	inline float	GetSin		( Vector3D &b);	// returns sin(angle) with b
	inline float	GetUnitCos	( Vector3D &b);	// returns cos(angle) with b (both unit length)
	inline float	GetUnitSin	( Vector3D &b);	// returns sin(angle) with b (both unit length)

	// overloaded operators
	Vector3D	operator =	(Vector3D &b);
	int			operator ==	(Vector3D &b);
	Vector3D	operator +	(Vector3D &b);
	Vector3D	operator +=	(Vector3D &b);
	Vector3D	operator -	(Vector3D &b);
	Vector3D	operator -=	(Vector3D &b);
	Vector3D	operator *	(float s);
	Vector3D	operator *=	(float s);

	Vector3D	operator -	(void);			// returns the negative of a
	Vector3D	operator ~	(void);			// returns the unit version of a
	float		operator !	(void);			// returns the magnitude of a
	float		operator *	(Vector3D &b);	// returns the dot product of a and b
	Vector3D	operator %	(Vector3D &b);	// returns the cross product of a and b
	Vector3D	operator ^	(Vector3D &b);	// returns the projection of a onto b
};




struct Vertex3D
{
	Vector3D	objDef,		// default (x, y, z) position in object coordinates
				objCur,		// current (x, y, z) position in object coordinates
				worldDef,	// default (x, y, z) position in world coordinates
				worldCur,	// current (x, y, z) position in world coordinates
				cameraPos;	// current (x, y, z) distance from the camera

	POINT		pt2D;		// (x, y) position on the screen (converted from 3D)
};




#endif