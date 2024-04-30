// **********************************************************************************
// ***						
// *** FileName:		types.h
// *** Creation:		7/11/01
// *** Author:			Elijah Emerson
// ***
// *** Headers Needed:	NONE
// *** 
// *** Description:		The types.h file is the most commonly included file in my source
// ***					code. It defines all basic types as well as 
// ***					common macros and templates.
// ***					
// *** 
// ***********************************************************************************

#pragma once
#ifndef TYPES_H
#define TYPES_H


//**********************
//*** STD DATA TYPES ***
//**********************

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long		u64;

typedef char				s8;
typedef short				s16;
typedef int					s32;
typedef long				s64;

typedef float				f32;
typedef double				f64;

typedef unsigned char		byte;
typedef unsigned short int	word;
typedef unsigned long  int	dword;
typedef unsigned __int64	qword;

typedef signed __int64		longlong;

typedef unsigned char		uchar;
typedef unsigned int		uint;
typedef unsigned short int	ushort;
typedef unsigned long  int	ulong;
typedef unsigned __int64	ulonglong;


//***********************
//*** STANDARD MACROS ***
//***********************
#ifndef SAFE_DELETE
#define SAFE_DELETE(_x) { if(_x){ delete _x; _x=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(_x) { if(_x){ delete[] _x; _x=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(_x){ if(_x){ _x->Release(); _x=NULL; } }
#endif

/*
#define absf(x)	((x) >= 0.0f ? (x) : -(x))
#define CLAMP(_x,_min,_max)	( ((_x)>(_max)) ? (_max) : ((_x)<(_min)) ? (_min) : (_x) )

#define COLOR_ARGB(a,r,g,b) \
			((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define COLOR_RGBA(r,g,b,a) COLOR_ARGB(a,r,g,b)

#define COLOR_COLORVALUE(r,g,b,a) \
			COLOR_RGBA((DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f),(DWORD)((a)*255.f))
*/


//**************************
//*** STANDARD TEMPLATES ***
//**************************
template <class T> inline T_ABS	 (T& x)					{ return (x >= 0.0f ) ? x : -x; }
template <class T> inline T_MAX  (T& x, T& y)			{ return x > y ? x : y; }
template <class T> inline T_CLAMP(T& x,T& min,T& max)	{ return (x > max) ? max : (x < min) ? min: x ; }

//**************************
//*** STANDARD Functions ***
//**************************
//DWORD inline COLOR_FromInt	(u32 a,u32 r,u32 g,u32 b){ return ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff))); }
//DWORD inline COLOR_FromFloat(f32 a,f32 r,f32 g,f32 b){ return COLOR_FromInt((DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f),(DWORD)((a)*255.f)); }
//void StrToUpper( char *str, int iLen )	
//{ while(iLen--)str[iLen] = toupper(str[iLen]); }



//*******************
//*** ERROR TYPES ***
//*******************
//#define RET_OK		0
//#define RET_ERROR		1

/*
#define ERESULT					u32

#define ER_SUCCESS				0
#define ER_FAILURE				1
#define ER_INVALID_ARGUMENT		2
#define ER_OUT_OF_MEMORY		3
#define ER_NULL_POINTER			4
#define ER_MISSING_FILE			5
#define ER_END_OF_FILE			6
#define ER_UNEXPECTED			7

enum EResult { ER_Success = 0,
			   ER_Failure,
			   ER_Invalid_Argument,
			   ER_Out_Of_Memory,
			   ER_Null_Pointer,
			   ER_Missing_File,
			   ER_End_Of_File,
			   ER_Unexpected
			};
*/

//***********************
//*** ERESULT DEFINED ***
//***********************
#ifndef ERESULT_DEFINED
#define ERESULT_DEFINED

//Using enum so we can see the text in the debugger
enum ERESULT { ER_SUCCESS = 0,			// 0 == sucess
			   ER_FAILURE,				// > 0 == failure
			   ER_INVALID_ARGUMENT,		// one of the arguments is bad.
			   ER_OUT_OF_MEMORY,		// could not allocate memory
			   ER_NULL_POINTER,			// a needed pointer is null
			   ER_MISSING_FILE,			// missing file 
			   ER_END_OF_FILE,			// eof
			   ER_UNEXPECTED			// ?
			};

//enum HRETURN { RET_SUCCESS = 0,			// 0 == sucess

static char* g_strEResult[] = 
{
	"ER_Success",				// 0 == sucess
	"ER_Failure",				// > 0 == failure
	"ER_Invalid_Argument",		// one of the arguments is bad.
	"ER_Out_Of_Memory",			// could not allocate memory
	"ER_Null_Pointer",			// a needed pointer is null
	"ER_Missing_File",			// missing file 
	"ER_End_Of_File",			// eof
	"ER_Unexpected"				// ?
};
#endif


//*****************
//*** D3D8 DEFS ***
//*****************
#ifndef _D3D8_H_

struct IDirect3D8;
typedef struct IDirect3D8 *LPDIRECT3D8, *PDIRECT3D8;

struct IDirect3DDevice8;
typedef struct IDirect3DDevice8 *LPDIRECT3DDEVICE8, *PDIRECT3DDEVICE8;

struct IDirect3DVertexBuffer8;
typedef struct IDirect3DVertexBuffer8 *LPDIRECT3DVERTEXBUFFER8, *PDIRECT3DVERTEXBUFFER8;

#endif //_D3D8_H_


#ifndef __D3DX8MATH_H__

struct D3DXMATRIX;

#endif //__D3DX8MATH_H__

/*
//******************
//*** WINNT DEFS ***
//******************
#ifndef _WINNT_

typedef void *PVOID;

#ifdef STRICT
typedef void *HANDLE;
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#else
typedef PVOID HANDLE;
#define DECLARE_HANDLE(name) typedef HANDLE name
#endif
typedef HANDLE *PHANDLE;

#endif //end _WINNT_

//********************
//*** WINDOWS DEFS ***
//********************
#ifndef _WINDEF_

#define NULL	0

typedef unsigned char		BYTE;
typedef unsigned short int	WORD;
typedef unsigned long  int	DWORD;
typedef unsigned int		UINT;

typedef UINT WPARAM;
typedef s64 LPARAM;
typedef s64 LRESULT;


#ifndef WIN_INTERNAL
DECLARE_HANDLE( HWND );
#endif

#ifndef GDI_INTERNAL
DECLARE_HANDLE( HDC );
#endif

//MAKE_HANDLE( HWND );
//MAKE_HANDLE( HINSTANCE );
//MAKE_HANDLE( HICON );

typedef struct tagRECT
{
    long    left;
    long    top;
    long    right;
    long    bottom;
} RECT, *LPRECT;

#endif	//end _WINDEF_

//FRect's use floats instead of longs
struct FRECT
{
	f32		left;
	f32		top;
	f32		right;
	f32		bottom;
	FRECT() {}
	~FRECT(){}
	inline void Clear( void ) { left = right = bottom = top = 0.0f; }
	inline operator = ( RECT& rhs ) 
	{ left = (f32)rhs.left; right = (f32)rhs.right; bottom = (f32)rhs.bottom; top = (f32)rhs.top; }
	inline operator = ( const RECT& rhs ) 
	{ left = (f32)rhs.left; right = (f32)rhs.right; bottom = (f32)rhs.bottom; top = (f32)rhs.top; }

};

//little helper functions
inline void ClearRect( RECT&  rc ) { rc.left = rc.right = rc.bottom = rc.top = 0; } 

//Convert WinHandle to and from long and ulong
__inline u64	WinHandleToULong(const void *h ){ return( (u64) h ); }
__inline s64	WinHandleToLong	(const void *h ){ return( (s64) h ); }
__inline void*	ULongToWinHandle(const u64 h )	{ return((void *)(u64*) h ); }
__inline void*	LongToWinHandle	(const s64 h )	{ return((void *)(s64*) h ); }
//__inline void*	ULongToWinHandle(const u64 h )	{ return((void *)(UINT_PTR) h ); }
//__inline void*	LongToWinHandle	(const s64 h )	{ return((void *)(INT_PTR)  h ); }

*/



#endif //end TYPES_H