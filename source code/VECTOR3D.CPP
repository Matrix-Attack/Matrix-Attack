#include <math.h>
#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "vector.h"




inline float Vector3D::GetLength()
{
	return sqrtf( (x * x) + (y * y) + (z * z) );
}




inline float Vector3D::Dot(Vector3D &b)
{
	return (x * b.x) + (y * b.y) + (z * b.z);
}




inline Vector3D Vector3D::Cross(Vector3D &b)
{
	Vector3D cross	= {	(y * b.z) - (z * b.y),
						(z * b.x) - (x * b.z),
						(x * b.y) - (y * b.x) };
	return cross;
}




// returns the cosine of the angle between a and b
inline float Vector3D::GetCos(Vector3D &b)
{
	return Dot(b) / ( GetLength() * b.GetLength() );
}




// returns the sine of the angle between a and b
inline float Vector3D::GetSin(Vector3D &b)
{
	Vector3D	ACrossB	= Cross(b);

	return ACrossB.z > 0 ?	 ACrossB.GetLength() / ( GetLength() * b.GetLength() ):
							-ACrossB.GetLength() / ( GetLength() * b.GetLength() );
}




// returns the cosine of the angle between a and b
// assuming a and b are unit vectors
inline float Vector3D::GetUnitCos(Vector3D &b)
{
	return Dot(b);
}




// returns the sine of the angle between a and b
// assuming a and b are unit vectors
inline float Vector3D::GetUnitSin(Vector3D &b)
{
	Vector3D	ACrossB	= Cross(b);

	return ACrossB.z > 0 ?	 ACrossB.GetLength():
							-ACrossB.GetLength();
}




// assigns the values of a to b
Vector3D Vector3D::operator = (Vector3D &b)
{
	x = b.x;
	y = b.y;
	z = b.z;

	return *this;
}




// determines if a is equivalent to b
int Vector3D::operator == (Vector3D &b)
{
	return ( (x == b.x) && (y == b.y) && (z == b.z) );
}




// returns the result of a + b
Vector3D Vector3D::operator + (Vector3D &b)
{
	Vector3D a = {	x + b.x,
					y + b.y,
					z + b.z };

	return a;
}




// returns the result of a + b
Vector3D Vector3D::operator += (Vector3D &b)
{
	x += b.x;
	y += b.y;
	z += b.z;

	return *this;
}




// returns the result of a - b
Vector3D Vector3D::operator - (Vector3D &b)
{
	Vector3D a = {	x - b.x,
					y - b.y,
					z - b.z };

	return a;
}




// returns the result of a - b
Vector3D Vector3D::operator -= (Vector3D &b)
{
	x -= b.x;
	y -= b.y;
	z -= b.z;

	return *this;
}




// returns a scalar multiple of the vector
Vector3D Vector3D::operator * (float s)
{
	Vector3D a = {	x * s,
					y * s,
					z * s };

	return a;
}




// returns a scalar multiple of the vector
Vector3D Vector3D::operator *= (float s)
{
	x *= s;
	y *= s;
	z *= s;

	return *this;
}




// returns the negative of a
Vector3D Vector3D::operator - (void)
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}




// returns the unit version of a
Vector3D Vector3D::operator ~ (void)
{
//	return (*this) * (1 / GetMagnitude() );
	return (*this) * (1 / !(*this) );
}




// returns the magnitude of a
float Vector3D::operator ! (void)
{
	return sqrtf( (x * x) + (y * y) + (z * z) );
}




// returns the dot product of a and b
float Vector3D::operator * (Vector3D &b)
{
	return (x * b.x) + (y * b.y) + (z * b.z);
}




// returns the cross product of a and b
Vector3D Vector3D::operator % (Vector3D &b)
{
	Vector3D a = {	(y * b.z) - (z * b.y),
					(z * b.x) - (x * b.z),
					(x * b.y) - (y * b.x) };

	return a;
}




// returns the projection of a onto b
Vector3D Vector3D::operator ^ (Vector3D &b)
{
	return b * ( (*this * b) / (b * b) );
}