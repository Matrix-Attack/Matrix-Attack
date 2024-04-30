// **********************************************************************************
// ***						
// *** FileName:		win_utilities.h
// *** Creation:		7/11/01
// *** Author:			Elijah Emerson
// ***
// *** Headers Needed:	"types.h"
// *** 
// *** Description:		Win utilities abstracts "windows.h" specific functions from the
// ***					the user so you don't have to include "windows.h". Common utility
// ***					functions are included as well.
// ***					
// *** 
// ***********************************************************************************

#pragma once
#ifndef WIN_UTILITIES_H
#define WIN_UTILITIES_H

//****************************
//*** Forward declarations ***
//****************************

#ifndef _FILE_DEFINED
struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };
typedef struct _iobuf FILE;
#define _FILE_DEFINED
#endif

struct tagRECT;



//*************************************************
//************** ERROR UTILITIES ******************
//*************************************************

class CErrorManager
{
private:
	char	m_strErrorMsg[512];		// message from the user
	char	m_strFileName[256];		// file name where the first error took place
	char	m_strFuncName[128];		// function name string
	u32		m_iLine;				// line number in code where the error occured
	bool	m_bErrorHit;			// did we hit an error yet?
	
			CErrorManager	( void ) : m_iLine(0), m_bErrorHit(false) { }
public:
			~CErrorManager	( void ) { }
	static	CErrorManager&	
			Instance		( void )	{ static CErrorManager Obj; return Obj; }
	void	HandleMessage	( const char* str, ...);
	void	HandleError		( const char* strFileName, u32 iLine, const char* strFuncName);
};

//*** Error Handeling utility functions***
void	DisplayLastWinError	( void );
void	DebugOutput			( char *str, ...);
void	ErrorMessage		( char *str, ...);


// *** Error Macros 
#define MANUAL_BREAKPOINT	_asm { int 3 }
#define ErrorLog( _str )				{	CErrorManager::Instance().HandleMessage( (_str) ); \
											CErrorManager::Instance().HandleError(__FILE__, __LINE__, NULL); } //__FUNCTION__); }
#define IF_RET_BREAK( _ret )				{ if( (_ret) ) break; }
#define IF_RET_MSG( _ret, _msg )			{ if( (_ret) ) { ErrorLog( (_msg) ); } }
#define IF_RET_MSG_AND_BREAK( _ret, _msg )	{ if( (_ret) ) { ErrorLog( (_msg) ); break; } }


// *** ERESULT Functions and macros
char*	EEGetErrorString( ERESULT er );
	
//Displays a message box, and passes the error code to EEGetErrorString.
ERESULT EETrace			( char *strFile, DWORD dwLine, ERESULT er, char* strMsg, bool bPopMsgBox );

//Error Handling Macros
#define	EETRACE_MSG(str)			EETrace( __FILE__, (DWORD)__LINE__, 0, str, FALSE)
#define	EETRACE_ERR(str,er)			EETrace( __FILE__, (DWORD)__LINE__,er, str, TRUE )
#define	EETRACE_ERR_NOMSGBOX(str,er)EETrace( __FILE__, (DWORD)__LINE__,er, str, FALSE)




//***************************************************
//************** WINDOWS UTILITIES ******************
//***************************************************

void	Util_PostQuitMessage	( void );
void	Util_TimeGetTime		( void );
void	Util_GetTimeAndDate		( char *output );
long	Util_DefWindowProc		( u32 hWnd, u32 iMsg, u32 wParam, long lParam);
long	Util_SetWindowLong		( u32 hWnd, int nIndex, long dwNewLong );
int		Util_SetWindowPos		( u32 hWnd, u32 hWndInsertAfter, 
								  int x, int y, int cx, int cy, u32 uFlags);
int		Util_GetWindowRect		( u32 hWnd, RECT* pRect);
int		Util_GetClientRect		( u32 hWnd, RECT* pRect);
u32		Util_GetActiveWindow	( void );	//returns an HWND as a u32
int		Util_SetCursorPos		( int x, int y);
void	Util_TextOut			( u32 hDC, int x, int y, const char* str, int iStrLen );
void	Util_ReplaceCharInString( char* str, char c, char n );
char*	Util_StrTok				( char* strToken, char** pNext );

//void Info_RemoveKey				( char *s, const char *key );
//void Util_RemoveKeyFromString	( char *s, const char *key );

//**************************************************
//************** STRING UTILITIES ******************
//**************************************************

ERESULT	String_GetString( const char *input, char *result );

//*******************************************************
//************** FILE IN/OUT UTILITIES ******************
//*******************************************************

//*******************************************************
//************** FILE IN/OUT UTILITIES ******************
//*******************************************************

#define MAX_NUM_PATHS	16
#define MAXCHAR_PATH	256

struct SFileInfo
{
	char	m_strFileName[MAXCHAR_PATH];
	char	m_strPathAndName[MAXCHAR_PATH];
};


class CFileList
{
private:
	// *** File and Directroy Data
	u32			m_iNumFiles;		//total number of files in list.
	u32			m_iNumDirectories;	//total number of directorys in list.
	char**		m_pFileList;		//2D array because we need a "single" array of character strings
	char**		m_pDirList;			//2D array because we need a "single" array of character strings
	
	u32			m_iNumPaths;
	char		m_strPathList[MAX_NUM_PATHS][MAXCHAR_PATH];
	
	SFileInfo*	m_pFileInfoList;	//A malloced array of FileInfo structs
	
	// *** Private Base Functions
	void		Clear				( void );
	ERESULT		CountFiles			( const char* strPath, const char* strDir, bool bRecursive );
	
public:
	// *** Public Base Functions
				CFileList			( void )	{ Clear();	}
				~CFileList			( void )	{ Destroy();}
	ERESULT		Destroy				( void );
	
	// *** Public Init Functions
	ERESULT		LoadLists			( const char *strPath, bool bRecursive = false );
	
	// *** Get Functions
	const char* GetFileName			( u32 i ) const; 
	const char* GetPathAndFileName	( u32 i ) const; 
	inline u32	GetNumFiles			( void )	{ return m_iNumFiles;		 }
	inline u32	GetNumDirectories	( void )	{ return m_iNumDirectories; }
};

//***File IN/OUT utility functions***

ERESULT	LoopUntilString				(FILE *fp, const char *str);
ERESULT	File_GetString				(FILE *fp, char *result);
ERESULT	File_GetString_NoQuotes		(FILE *fp, char *result);
ERESULT	File_GetInt					(FILE *fp, s32  *result);
ERESULT	File_GetInt					(FILE *fp, u32  *result);
ERESULT	File_GetFloat				(FILE *fp, f32  *result);
ERESULT	File_GetDouble				(FILE *fp, f64  *result);
//ERESULT		File_GetScalar		(FILE *fp, scalar *result);

ERESULT	File_GetIntAfterString		(FILE *fp, const char *string, s32 *result);
ERESULT	File_GetIntAfterString		(FILE *fp, const char *string, u32 *result);
ERESULT	File_GetFloatAfterString	(FILE *fp, const char *string, f32 *result);
ERESULT	File_GetStringAfterString	(FILE *fp, const char *string, char*result);



#endif