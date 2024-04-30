// ********************************************************
// ***						
// *** FileName:    win_utilities.cpp
// *** Creation:    7/11/01
// *** Author:	    Elijah Emerson
// *** Description: for all your windows utilities needs, 
// ***				include win_utilities.h instead of that slug windows.h 
// ***				
// ********************************************************

#define WIN32_LEAN_AND_MEAN
#include <windows.h>			//for window's function wrappers and the WIN32_FIND_DATA definition
#include <winuser.h>
#include <mmsystem.h>			//for file search functions
#include <stdio.h>				//for sprintf
#include <stdlib.h>

#include <string>
using namespace std;

#include ".\mmgr.h"
#include ".\types.h"			//standard type definitions
#include ".\win_utilities.h"	//for CFileList definition

/*
typedef struct tagWINDOWINFO
{
    DWORD cbSize;
    RECT  rcWindow;
    RECT  rcClient;
    DWORD dwStyle;
    DWORD dwExStyle;
    DWORD dwOtherStuff;
    UINT  cxWindowBorders;
    UINT  cyWindowBorders;
    ATOM  atomWindowType;
    WORD  wCreatorVersion;
} WINDOWINFO, *PWINDOWINFO, *LPWINDOWINFO;

BOOL WINAPI
GetWindowInfo(
    HWND hwnd,
    PWINDOWINFO pwi
);
*/
void CenterWindow( HWND hWindow )
{
	
//	WINDOWINFO winInfo;
	
//	winInfo.cbSize = sizeof(WINDOWINFO);
//	GetWindowInfo( GetActiveWindow(), &winInfo );

	//Move our dialog box to the center of the screen
	RECT	rcWindow, rcWork;

	SystemParametersInfo( SPI_GETWORKAREA, 0, (LPVOID) &rcWork, 0);
	GetWindowRect(hWindow, &rcWindow);

	int iWidth	= rcWindow.right  - rcWindow.left;
	int iHeight = rcWindow.bottom - rcWindow.top;

	MoveWindow(	hWindow, 
				(rcWork.right  - rcWork.left - iWidth ) >> 1,	  
				(rcWork.bottom - rcWork.top  - iHeight) >> 1, 
				iWidth,	
				iHeight,
				true);
}

//**** These globals have to do with our writeError function
//Error Settings
#define WIN_UTIL_DEBUG		0	//if DEBUG == 1 then the user can choose to break into our code
#define BREAK_ON_ERROR		1
#define ERROR_LOG_FILENAME	("error_log.txt")


void CErrorManager::HandleMessage(const char *str, ...)
{
	if( (str == 0) || (str[0] == 0) )		//Check to make sure we have a string
		return;

	va_list	pArgList;						//Pointer to our list of arguments

	va_start(pArgList, str);				//Start our list of agruments
	vsprintf(m_strErrorMsg, str, pArgList);	//Put our formated string into the buffer and get the size
	va_end(pArgList);						//End our argument list
	
	//log error
	FILE* fp = fopen(ERROR_LOG_FILENAME, "at");
	if(fp)
	{
		fprintf(fp, m_strErrorMsg);
		fclose(fp);
	}
}

void CErrorManager::HandleError( const char* strFileName, u32 iLine, const char* strFuncName)
{
	char	strBuffer[512];
	FILE*	fp;
	
	
	//Is this our first time hitting an error?
	if( m_bErrorHit )
	{
		//copy our main information into the string buffer
		sprintf(strBuffer, "File:\t%s\nLine:\t%d\nFunc:\t%s\nMsg:\t%s\n",
						(strrchr(strFileName,'\\'))+1,	// file
						iLine,							// line
						strFuncName,					// function name
						m_strErrorMsg);					// error message
		
		
		//log error
		fp = fopen(ERROR_LOG_FILENAME, "at");
		if(fp)
		{
			fprintf(fp, strBuffer);
			fclose(fp);
		}
		
		//We have already hit an error... so instead of strcpy, strcat our history.
//		strcpy(strBuffer, strLocation);
//		strcat(strBuffer, "->");
//		strcat(strBuffer, strHistory);
		
/*		if(bEndOfHistory)
		{
			strcat(strBuffer, "\n********************************************************\n");
			strcat(strBuffer, strErrorMsg);
#if WIN_UTIL_DEBUG
			strcat(strBuffer, "\n Do you wish to break into the code?");
			if( IDYES == MessageBox(NULL, strBuffer, "Eli's CRAZY Error log (Debug: 1, BOE: 0)", MB_YESNO|MB_ICONEXCLAMATION) )
			{
				 _asm { int 3 }
			}
#else
			//If we are not in WIN_UTIL_DEBUG mode then don't allow the user to break into the code
			MessageBox(NULL, strBuffer, "Eli's CRAZY Error log (Debug: 0, BOE: 0)", MB_OK|MB_ICONEXCLAMATION);
#endif
		}
		else
		{
			//If our History string length (starting from the last new line to the end of string)
			// is larger than 50 characters then add another "new line".
			//if( strlen( strrchr(strBuffer, '\\') ) > 75 )
			//	strcat(strBuffer, "\n");
			
//			char *p;
//			p = strrchr(strBuffer, '\\');
//			if( p[1] == 'n' && strlen(p) > 75)
//				strcat(strBuffer, "\n");
			
			//copy our strBuffer into our history because we need to save our history
			strcpy(strHistory, strBuffer);
		}
*/	}
	else
	{
		//Set the ErrorHit boolean
		m_bErrorHit = true;
		

		// Set our error information
		
		//(strrchr(m_strFilename,'\\'))+1
		if(strFileName && strFileName[0])
			strcpy(m_strFileName, strFileName);
		else
			strcpy(m_strFileName, "NONE");
		
		if(strFuncName && strFuncName[0])
			strcpy(m_strFuncName, strFuncName);
		else
			strcpy(m_strFuncName, "NONE");

		m_iLine = iLine;
		
		
		//This is our first time calling the HandleError function.
		// First open an error log file and put the details into that text file.

		// so lets save our error message untill we have a completed history.
		// pChar = strrchr(strFile, '\\'); //find last occurence
		char strMsg[512];
		sprintf(strMsg, 
				"File:\t%s\nLine:\t%d\nFunc:\t%s\nMsg:\t%s\n",
				m_strFileName,		// file
				m_iLine,			// line
				m_strFuncName,		// function name
				m_strErrorMsg);		// error message

		//sprintf(strHistory, "%s", strLocation);
		

#if WIN_UTIL_DEBUG
#if BREAK_ON_ERROR
		//strcat(strBuffer, strHistory);
		strcpy(strBuffer, "\n********************************************************\n");
		strcat(strBuffer, strMsg);
		strcat(strBuffer, "\n Do you wish to break into the code?");
		if( IDYES == MessageBox(NULL, strBuffer, "Eli's CRAZY Error log (Debug:1, BOE: 1)", MB_YESNO|MB_ICONEXCLAMATION) )
		{
			// If you got here because you want to debug, you will need to go
			// back on the callstack.
			MANUAL_BREAKPOINT;
		}
#endif //end BREAK_ON_ERROR
#endif //end WIN_UTIL_DEBUG
		
		//log error
		fp = fopen(ERROR_LOG_FILENAME, "wt");
		if(fp)
		{
			fprintf(fp, strBuffer);
			fclose(fp);
		}

	}
}

//Wrapper for OutputDebugString function
//**********************************************************************************
// AUTHOR(S):	Elijah Emerson
// FUNCTION:	DebugOutput
// PARAMATERS:	FILE *str,			- message displayed inside our DebugWindow in vc.
//
// RETURNS:		void
//
// DESCRIPTION:	Wrapper for OutputDebugString function
//
//**********************************************************************************
void DebugOutput(char *str, ...)
{
	va_list	pArgList;						//Pointer to our list of arguments
	char	buffer[512];					//Temporary buffer to hold the text

	if((str == 0)||(str == '\0'))			//Check to make sure we have a string
		return;

	va_start(pArgList, str);				//Start our list of agruments
	vsprintf(buffer, str, pArgList);		//Put our formated string into the buffer and get the size
	va_end(pArgList);						//End our argument list

	//Now that we formated our string pass it into our debug window.
	OutputDebugString(buffer);
}

void ErrorMessage(char *str, ...)
{
	va_list	pArgList;						//Pointer to our list of arguments
	char	buffer[512];					//Temporary buffer to hold the text
	
	if((str == 0)||(str == '\0'))			//Check to make sure we have a string
		return;
	
	va_start(pArgList, str);				//Start our list of agruments
	vsprintf(buffer, str, pArgList);		//Put our formated string into the buffer and get the size
	va_end(pArgList);						//End our argument list
	
	// log our error
	CErrorManager::Instance().HandleMessage( buffer );

	//Now that we formated our string pass it into our debug window.
	MessageBox(NULL, buffer, "Error Message", MB_OK);
}

void DisplayLastWinError( void )
{
	LPVOID lpMsgBuf;
	
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	
	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Win32 Error", MB_OK | MB_ICONINFORMATION );
	
	// Free the buffer.
	LocalFree( lpMsgBuf );
}

int Util_GetWindowRect(u32 hwnd, RECT *pRect)
{
	return GetWindowRect((HWND__ *)hwnd, (tagRECT *)pRect);
}

int Util_GetClientRect(u32 hWnd, RECT *pRect)
{
	return GetClientRect((HWND__ *)hWnd, pRect );
}

u32 Util_GetActiveWindow( void )
{
	return (u32)GetActiveWindow( );
}

int Util_SetCursorPos(int x, int y)
{
	return SetCursorPos(x, y);
}

void Util_RemoveKeyFromString( char *s, const char *key )
{
	char buffer[1024];
	char *c;
	
	strcpy(buffer, s);
	
	if( (c = strstr(buffer, key)) == 0)
		return;	
	
	c[0] = '\0';

	c = strstr(s, key);
	strcat(buffer, c + strlen(key) );

	//our new buffer should have the key removed so make s == buffer
	strcpy(s, buffer);
}

void Util_PostQuitMessage( void )
{
	PostQuitMessage( 0 );
}

void Util_GetTimeAndDate( char *output )
{
	SYSTEMTIME SystemTime;

//	memset(lpSystemTime, 0, sizeof( SYSTEMTIME ) );
	if(strlen(output) <= 16 )
		return;//not enough room

	GetLocalTime( &SystemTime );
	
	sprintf(output, "%d/%d/%d %d:%d",
		SystemTime.wMonth, SystemTime.wDay, SystemTime.wYear, 
		SystemTime.wMinute, SystemTime.wSecond );
}

void Util_TextOut( u32 hDC, int x, int y, const char* str, int iStrLen )
{
	TextOut((HDC__ *)hDC, x, y, str, iStrLen);
}

//**********************************************************************************
//	FUNCTION NAME:	Util_ReplaceCharInString
//	AUTHOR(S):		Elijah Emerson
//	CREATION DATE:	8/2/2001
//	
//	ARGUMENT 1:		char* str	- string to look for character "c"
//	ARGUMENT 2:		char c		- character to look for 
//	ARGUMENT 3:		char n		- new character to replace "c" with
//	RETURN TYPE:	void 		- 
//	DESCRIPTION:	Replaces all characters in the string of var "c" into var "n".
//**********************************************************************************
void Util_ReplaceCharInString( char* str, char c, char n )
{
	int	iLen;
	
	if(str == NULL)	return;
	
	iLen = strlen(str);
	while(iLen--)
	{
		if( str[iLen] == c) 
			str[iLen] = n;
	}
}

//**********************************************************************************
//	FUNCTION NAME:	Util_StrTok
//	AUTHOR(S):		Elijah Emerson
//	CREATION DATE:	7/22/2001
//	
//	ARGUMENT 1:		char* strToken	- (in) pointer to string to look at for tokens
//	ARGUMENT 2:		char** pNext	- (in/out) address of pointer to next token found 
//	RETURN TYPE:	char* 			- pointer to first token found
//	DESCRIPTION:	Handy dirivitave of strtok, this function allows quotable tokens
//**********************************************************************************
char* Util_StrTok( char* strToken, char** pNext )
{
	// Start of next token (if there is one)
	char *pStart;
	
	// If NULL is passed in, continue searching
	if( strToken == NULL )
	{
		if( *pNext != NULL )
		{
			strToken = *pNext;
		} 
		else 
		{
			return NULL;// Reached end of original string
		}
	}
	
	// Look for commented out text to skip
	while( ( *strToken == '/' && *(strToken+1) == '/' ) )
	{
		// We found some commented out text. Which means that all text untill the '\n'
		// is not concidered a token. So loop untill end of string delimiter is found.
		while( *strToken != '\n' && *strToken != 0 )
		{
			++strToken;
		}
	}

	// Skip leading whitespace before next token
	while(	(*strToken == ' ') || (*strToken == '\t') || (*strToken == '\n') ) 
	{
		++strToken;
	}

	// Zero length string, so no more tokens to be found
	if ( *strToken == 0 ) 
	{
		*pNext = NULL;
		return NULL;
	}

	// Look for a quoted token
	if ( *strToken == '\"' ) 
	{
		//skip the first quote char
		++strToken;
		
		pStart = strToken;
		
		// Find ending quote or end of string
		while ( (*strToken != '\"') && (*strToken != 0) ) 
		{
			++strToken;
		}
		
		// Check for end of string
		if ( *strToken == 0 ) 
		{
			*pNext = NULL;
		} 
		else 
		{
			// More to find, note where to continue searching
			*strToken = 0;
			*pNext = strToken + 1;
		}
		
	} 
	else 
	{
		// Token not in quotes
		pStart = strToken;
		
		// Find next whitespace delimiter or end of string
		while ( (*strToken != 0)	&& (*strToken != ' ')  && 
			    (*strToken != '\t') && (*strToken != '\n') ) 
		{
			++strToken;
		}
		
		// Reached end of original string?
		if ( *strToken == 0 ) 
		{
			*pNext = NULL;
		} 
		else 
		{
			*strToken	= 0;
			*pNext		= strToken + 1;
		}
		return pStart;
	}
	
	// Return ptr to start of token found
	return pStart;
}

//**********************************************************************************
// FUNCTION:	String_GetString
// PARAMATERS:	FILE *fp,			- file pointer that is open with flags "rw"
//				char *str,			- character string to fill in once we find a
//									  string in quotes.
//
// RETURNS:		u32					- 0 == OK, 1 == ERROR
//
// DESCRIPTION:	This function will look for the next quote symbol '"' and once it
//				finds that quote symbol it will read in characters untill either 
//				another	quote symbol or a return symbol is found.
//
//**********************************************************************************
ERESULT String_GetString(const char *input, char *result)
{
	if(input == NULL) return ER_INVALID_ARGUMENT;

	char *pos;
	
	//Find the beggining of our string by looking for a quote symbol '"'.
	pos = strchr(input, '"');

	if(!pos)
	{
		DebugOutput("\n***Error looking for \" in string %s!!!\n", input);
		return ER_FAILURE;
	}
	//Copy our new found string untill we hit a '"' or the end of file marker.
	pos++;
	while( pos[0] != '"')
	{
		*result = pos[0];
		result++;
		
		pos++;

		//If we find a /" then disregaurd the " 
		if(pos[0] == '"' && result[-1] == '/')
		{
			result[-1] = '"';
			pos++;
		}
	}
	*result = '\0'; //put null at the end of our string

	return ER_SUCCESS;//success
}

//**********************************************************************************
// FUNCTION:	LoopUntilString
// PARAMATERS:	FILE *fp,			- file pointer that is open with flags "rw"
//				char *str,			- character string to fill in once we find a
//									  string in quotes.
//
// RETURNS:		ERESULT				- 0 == OK, 1 == ERROR
//
// DESCRIPTION:	This function searches a text file for the first occurrence of
//				<string> starting from the current position of the file pointer.
//
//**********************************************************************************
ERESULT LoopUntilString(FILE *fp, const char *str)
{
	if(!fp || !str ) return ER_INVALID_ARGUMENT;

	char buffer[256];
	s32	 length;
	
	length = strlen(str);
	if(length >= 256)
		return ER_INVALID_ARGUMENT;

	buffer[length]	= '\0';

	// read <length> characters at a time, then do a strcmp
	for(s32 i = 0; i < length; i++)
	{
		buffer[i] = getc(fp);
		if(feof(fp) )
		{
			DebugOutput("\n*** Error looking for tag %s in LoopUntilSuck, er..String *** \n", str);
			return ER_END_OF_FILE;
		}
	}
	
	// this loop will only execute if the strings didn't match the first time
	while(strcmp(buffer, str))
	{
		// move all the letters in buffer back by one space,
		// then read a new character
		for(i = 0; i < length - 1; i++)
			buffer[i]		= buffer[i + 1];

		buffer[length - 1]	= getc(fp);
		if(feof(fp) )
		{
			DebugOutput("\n*** Error looking for tag %s in LoopUntilSuck, er..String *** \n", str);
			return ER_END_OF_FILE; //return failure
		}
	}

	return ER_SUCCESS;//return success
}

//**********************************************************************************
// FUNCTION:	File_GetString
// PARAMATERS:	FILE *fp,			- file pointer that is open with flags "rw"
//				char *str,			- character string to fill in once we find a
//									  string in quotes.
//
// RETURNS:		ERESULT					- 0 == OK, 1 == ERROR
//
// DESCRIPTION:	This function will look for the next quote symbol '"' and once it
//				finds that quote symbol it will read in characters untill either 
//				another	quote symbol or a return symbol is found.
//
//**********************************************************************************
ERESULT File_GetString(FILE *fp, char *result)
{
	if(fp == NULL) return ER_INVALID_ARGUMENT;
	
	char c, *p = result;
	
	//Find the beggining of our string by looking for a quote symbol '"'.
	while( ( c = getc(fp) ) != '"' )
	{
		if(feof(fp)	)	
		{
			DebugOutput("*** Unexpected eof in File_GetString()\n");
			return ER_END_OF_FILE;	//end of file found, return fail
		}
	}
	
	//Copy our new found string untill we hit a '"' or the end of file marker.
	
	c  = getc(fp);
	while( c != '"')
	{
		*p = c;
		p++;
		
		c  = getc(fp);

		//If we find a /" then disregaurd the " 
		if(c == '"' && p[-1] == '/')
		{
			p[-1] = '"';
			c = getc(fp);
		}
		
		if(feof(fp))
		{
			DebugOutput("*** Unexpected eof in File_GetString()\n");
			return ER_END_OF_FILE;	//end of file found, return fail
		}
	}
	*p = '\0'; //put null at the end of our string

	return ER_SUCCESS;//success
}

ERESULT File_GetString_NoQuotes(FILE *fp, char *result)
{
	if(fp == NULL) return ER_INVALID_ARGUMENT;

	s32 i = 0;
	char c;

	//***Find the BEGINING***
	//go through string untill we find a valid symbol
	while( ((c = getc(fp)) < '!' ) || c > 'z' || c == '"')
	{
		if(feof(fp)	)
		{
			DebugOutput("***Unexpected eof in File_GetString_NoQuotes()\n");
			return ER_END_OF_FILE;	//end of file found, return fail
		}
	}

	//since we already found 1 valid character lets assign that to our buffer
	result[i++] = c;

	//***Find the END***
	while( (c = getc(fp)) >= '!' && c <= 'z' && c != '"')//!= '\n' &&	c != '\t' && c != '\r' && c != ' ' ) 
	{
		if(feof(fp))
		{
			DebugOutput("***Unexpected eof in File_GetString_NoQuotes()\n");
			return ER_END_OF_FILE;	//end of file found, return fail
		}
		result[i++] = c;
	}

	//terminate our string
	result[i++] = NULL;
	
	return ER_SUCCESS;//success
}

//**********************************************************************************
// FUNCTION:	File_GetInt
// PARAMATERS:	FILE *fp,			- file pointer that is open with flags "rw"
//				s32  *result,		- pointer to result that we are going to fill.
//
// RETURNS:		ERESULT					- 0 == OK, 1 == ERROR
//
// DESCRIPTION:	This function will look for the next number, or '-' sign in the
//				file pointer and then read in that number and fill in the *result pointer.
//
//**********************************************************************************
ERESULT File_GetInt(FILE *fp, s32 *result)
{
	if(fp == NULL) return ER_INVALID_ARGUMENT;

	char	c;
	//***Find the BEGINING of our number***
	while( ( ((c = getc(fp)) < '0') || c > '9') && c != '-' )
	{
		if(feof(fp))
		{
			DebugOutput("***Unexpected eof in File_GetInt()\n");
			return ER_END_OF_FILE;	//end of file found, return fail
		}
	}
	fseek(fp, -1, SEEK_CUR);
	fscanf(fp, "%d", result);
	return ER_SUCCESS; //return success
}

//**********************************************************************************
// FUNCTION:	File_GetInt
// PARAMATERS:	FILE *fp,			- file pointer that is open with flags "rw"
//				u32  *result,		- pointer to result that we are going to fill.
//
// RETURNS:		ERESULT					- 0 == OK, 1 == ERROR
//
// DESCRIPTION:	This function will look for the next number, or '-' sign in the
//				file pointer and then read in that number and fill in the *result pointer.
//
//**********************************************************************************
ERESULT File_GetInt(FILE *fp, u32 *result)
{
	if(fp == NULL) return ER_INVALID_ARGUMENT;

	char	c;
	//***Find the BEGINING of our number***
	while( ( ((c = getc(fp)) < '0') || c > '9') && c != '-' )
	{
		if(feof(fp)	)	
		{
			DebugOutput("***Unexpected eof in File_GetInt()\n");
			return ER_END_OF_FILE;	//end of file found, return fail
		}
	}
	fseek(fp, -1, SEEK_CUR);
	fscanf(fp, "%d", result);
	return ER_SUCCESS; //return success
}

//**********************************************************************************
// FUNCTION:	File_GetFloat
// PARAMATERS:	FILE *fp,			- file pointer that is open with flags "rw"
//				f32  *result,		- pointer to result that we are going to fill.
//
// RETURNS:		ERESULT				- 0 == OK, 1 == ERROR
//
// DESCRIPTION:	This function will look for the next number, or '-' sign in the
//				file pointer and then read in that number and fill in the *result pointer.
//
//**********************************************************************************
ERESULT File_GetFloat(FILE *fp, f32 *result)
{
	if(fp == NULL) return ER_INVALID_ARGUMENT;

	char	c;
	//***Find the BEGINING of our number***
	while( ( ((c = getc(fp)) < '0') || c > '9') && c != '-' )
	{
		if(feof(fp))
		{
			DebugOutput("***Unexpected eof in File_GetFloat()\n");
			return ER_END_OF_FILE;	//end of file found, return fail
		}
	}
	fseek(fp, -1, SEEK_CUR);
	fscanf(fp, "%f", result);
	return ER_SUCCESS; //success
}

ERESULT File_GetDouble(FILE *fp, f64 *result)
{
	if(fp == NULL)	return ER_INVALID_ARGUMENT;

	char	c;
	f32 f;

	//***Find the BEGINING of our number***
	while( ( ((c = getc(fp)) < '0') || c > '9') && c != '-' )
	{
		if(feof(fp))
		{
			DebugOutput("***Unexpected eof in File_GetDouble()\n");
			return ER_END_OF_FILE;	//end of file found, return fail
		}
	}
	fseek(fp, -1, SEEK_CUR);
	fscanf(fp, "%f", &f);

	*result = f;

	return ER_SUCCESS; //success
}


//**************************************************************
//** These functions are here to make life easy... thats all. **
//**************************************************************
ERESULT File_GetIntAfterString(FILE *fp, const char *string, s32 *result)
{
	if(fp == NULL)	return ER_INVALID_ARGUMENT;
	ERESULT ret = LoopUntilString(fp, string);
	return ( ret != ER_SUCCESS ) ? ret : File_GetInt(fp, result);
}

ERESULT File_GetIntAfterString(FILE *fp, const char *string, u32 *result)
{
	if(fp == NULL)	return ER_INVALID_ARGUMENT;
	ERESULT ret = LoopUntilString(fp, string);
	return ( ret != ER_SUCCESS ) ? ret : File_GetInt(fp, result);
}

ERESULT File_GetFloatAfterString(FILE *fp, const char *string, f32 *result)
{
	if(fp == NULL)	return ER_INVALID_ARGUMENT;
	ERESULT ret = LoopUntilString(fp, string);
	return ( ret != ER_SUCCESS ) ? ret : File_GetFloat(fp, result);
}

ERESULT	File_GetStringAfterString(FILE *fp, const char *string, char *result)
{
	if(fp == NULL)	return ER_INVALID_ARGUMENT;
	ERESULT ret = LoopUntilString(fp, string);
	return ( ret != ER_SUCCESS ) ? ret : File_GetString(fp, result);
}

//*************************************************************************************
//********************************* CFileList *****************************************
//*************************************************************************************
//*************************************************************************************
//********************************* CFileList *****************************************
//*************************************************************************************

void CFileList::Clear()
{
	m_iNumFiles			= 0;
	m_pFileList			= NULL;

	m_iNumDirectories	= 0;	//total number of directorys in list.
	m_pDirList			= NULL;	//2D array because we need a "single" array of character strings
	
	m_iNumPaths			= 0;
//	m_strPathList

}

ERESULT CFileList::Destroy()
{
	u32 i;
	
	if( m_pFileInfoList )
	{
		delete[] m_pFileInfoList;
		m_pFileInfoList = NULL;
	}
	
	if( m_pDirList )
	{
		for(i=0; i < m_iNumDirectories; i++)
		{
			free(m_pDirList[i]);
			m_pDirList[i] = NULL;
		}
		free(m_pDirList);
		m_pDirList = NULL;
	}

	Clear();

	return ER_SUCCESS;
}

ERESULT CFileList::CountFiles( const char* strPath, const char* strDir, bool bRecursive )
{
	WIN32_FIND_DATA	FileData;
	HANDLE			hSearch;
	u32				bKeepworking = true;
	
	if(strPath == NULL || strlen(strPath) >= 1024)
		return ER_INVALID_ARGUMENT;
	
	// *** Tolkenize the strPath...
	u32		iStartPath, iEndPath, iCurPath;
	char	strBuffer[1024];
	char	strTest[256];
	char*	pStart;
	char*	pToken;
	char*	pNext;
	
	iStartPath	= m_iNumPaths;	
	
	//make sure we only manipulate our memory
	strcpy(strBuffer, strPath);
	pStart		= strBuffer;
	
	do
	{
		pToken = Util_StrTok(pStart, &pNext);
		pStart = pNext;

		// This function only supports up to 16 paths
		if(pToken && m_iNumPaths < MAX_NUM_PATHS )
		{
			//Make sure our path fits in our string list
			if(strlen(pToken) > MAXCHAR_PATH-1)
				pToken[MAXCHAR_PATH-1] = NULL;
			
			if(strDir)	
			{
				strcpy(strTest, strDir);
				strcat(strTest, "\\");
			}
			else
			{
				strTest[0] = NULL;
			}
			strcat(strTest, pToken);
			
			//Test this token
			hSearch = FindFirstFile(strTest , &FileData );
			if ( hSearch == INVALID_HANDLE_VALUE )
				continue;		
			FindClose ( hSearch );

			//This token is good, store it in our string list
			strcpy(m_strPathList[m_iNumPaths], strTest);
			m_iNumPaths++;
		}
	}while(pToken != NULL);
	
	iEndPath = m_iNumPaths;

	//*** FIRST find out how many files and directories there are ***
	for(iCurPath = iStartPath; iCurPath < iEndPath; iCurPath++)
	{
		//Get the first file in our directory
		hSearch = FindFirstFile (m_strPathList[iCurPath], &FileData );
		if ( hSearch == INVALID_HANDLE_VALUE )
		{
			ErrorMessage("Invalid path -> %s", m_strPathList[iCurPath]);
			DisplayLastWinError();
			return ER_INVALID_ARGUMENT;
		}
		
		do
		{
			//Update our counter
			if( FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if(strcmp(FileData.cFileName, ".") && strcmp(FileData.cFileName, ".." ) )
				{
					m_iNumDirectories++;

					if(bRecursive)
					{
						CountFiles( strPath, FileData.cFileName, true);
					}
				}
			}
			else
			{
				m_iNumFiles++;
			}

			//Grab the next file in the directory
			bKeepworking = FindNextFile ( hSearch, &FileData );

			if ( !bKeepworking && ( GetLastError() == ERROR_NO_MORE_FILES ) )
				break;
		}while( 1 );
		
		//clean up
		FindClose ( hSearch );

	}//end for loop

	return ER_SUCCESS;
}

// strPath looks like --> "PowerUps\\Weapons\\*.wpn"  <--example from Gunsha
ERESULT CFileList::LoadLists(const char *strPath, bool bRecursive)
{
	WIN32_FIND_DATA	FileData;
	HANDLE			hSearch;
	u32				bKeepworking = true,
					i;
	u32				iCurPath;

	if(strPath == NULL || strlen(strPath) >= 1024)
		return ER_INVALID_ARGUMENT;
	
	
	// *** If we already have lists get rid of them.
	if(m_pFileList || m_pDirList )
		Destroy();
	

	//Count the files
	CountFiles(strPath, 0, bRecursive);
	
	//************************
	// now that we have our file and directory count lets store them
	//************************
	
	if( m_iNumFiles )
	{
		//*** Allocate enough memory for our fileList ***
		m_pFileInfoList = new SFileInfo[m_iNumFiles];
		
		//go through all the paths given
		i = 0;
		for(iCurPath = 0; iCurPath < m_iNumPaths; iCurPath++)
		{
			bKeepworking = true;

			//*** THIRD go through the file list again and copy our strings into our fileTable
			// Get the first file in our directory
			hSearch = FindFirstFile ( m_strPathList[iCurPath], &FileData );
			
			if ( hSearch == INVALID_HANDLE_VALUE ) 
			{
				return ER_FAILURE;
			}
			
			while ( 1 )
			{
				if( !(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
				{
					//save our filename
					strcpy(m_pFileInfoList[i].m_strFileName, FileData.cFileName );
					
					//save our directory
					string	strTemp;
					strcpy( m_pFileInfoList[i].m_strPathAndName, m_strPathList[iCurPath] );
					strTemp = m_strPathList[iCurPath];
					m_pFileInfoList[i].m_strPathAndName[ strTemp.find_last_of( "\\" ) + 1] = NULL;
					strcat( m_pFileInfoList[i].m_strPathAndName, FileData.cFileName );
					i++;
				}
				//Grab the next file in the directory
				bKeepworking = FindNextFile ( hSearch, &FileData );
				if ( !bKeepworking && ( GetLastError ( ) == ERROR_NO_MORE_FILES ) )
					break;
			}
			FindClose ( hSearch );

		}//end for loop
	}
	
/*	if( m_iNumDirectories )
	{
		//*** Allocate enough memory for our DirectoryList ***
		m_pDirList = new char*[m_iNumDirectories];//(char**)malloc(m_iNumDirectories * sizeof(char *) );
		
		if(m_pDirList == NULL)
			return ER_OUT_OF_MEMORY;

		for(i=0; i < m_iNumDirectories; i++)
		{
			m_pDirList[i] = new char[256];//(char*)malloc(256 * sizeof(char) );
			
			if(m_pDirList[i] == NULL)
				return ER_OUT_OF_MEMORY;
		}
		
		i = 0;
		for(iCurPath = 0; iCurPath < m_iNumPaths; iCurPath++)
		{
			bKeepworking = true;	
			
			//*** THIRD go through the file list again and copy our strings into our fileTable
			//Get the first file in our directory
			hSearch = FindFirstFile ( m_strPathList[iCurPath], &FileData );
			if ( hSearch == INVALID_HANDLE_VALUE )
			{
				return ER_FAILURE;
			}
			
			while ( 1 )
			{
				if( FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
					strcmp(FileData.cFileName, ".")						 && 
					strcmp(FileData.cFileName, "..")					 )
				{
					xPos = 0;
					while((m_pDirList[i][xPos] = m_strPathList[iCurPath][xPos]) != '*') ++xPos;
					m_pDirList[i][0] = '\0';
					strcat(m_pDirList[i], FileData.cFileName);
					i++;
				}
				
				//Grab the next file in the directory
				bKeepworking = FindNextFile ( hSearch, &FileData );
				if ( !bKeepworking && ( GetLastError ( ) == ERROR_NO_MORE_FILES ) )
					break;
			}
			FindClose ( hSearch );

		}//end for loop
	}
*/	
	return ER_SUCCESS;
}

const char* CFileList::GetFileName( u32 i ) const
{
	if(i > m_iNumFiles)
		return NULL;

	if( m_pFileInfoList == NULL)
		return NULL;

	return m_pFileInfoList[i].m_strFileName;
}


const char* CFileList::GetPathAndFileName( u32 i ) const
{
	if(i > m_iNumFiles)
		return NULL;

	if( m_pFileInfoList == NULL)
		return NULL;

	return m_pFileInfoList[i].m_strPathAndName;
}