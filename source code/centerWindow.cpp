
//#include <windows.h>

//#include <winuser.h>
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
typedef long LPARAM;
typedef long LRESULT;


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

void CenterWindow( HWND hWindow )
{
	
	WINDOWINFO winInfo;

	winInfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo( GetActiveWindow(), &winInfo );

	//Move our dialog box to the center of the screen
	RECT	rcClient, rcWork;

	SystemParametersInfo( SPI_GETWORKAREA, 0, (LPVOID) &rcWork, 0);
	GetClientRect(hDlg, &rcClient);

	int iWidth	= winInfo.rcClient.right - winInfo.rcClient.left;
	int iHeight = winInfo.rcClient.bottom - winInfo.rcClient.top;

	MoveWindow(	hWindow, 
				(rcWork.right  - rcWork.left - iWidth ) >> 1,	  
				(rcWork.bottom - rcWork.top  - iHeight) >> 1, 
				iWidth,	
				iHeight,
				true);
}
