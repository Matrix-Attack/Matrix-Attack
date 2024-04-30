/**************
tim berry
creation date:11-30-99
**************/
#include <windows.h>
#include ".\mmgr.h"
#define	TIMER_UPDATE_FREQUENCY	4

/**************
this updates every
TIMER_UPDATE_FREQUENCY
mil secs. the timer
will call the wnd
proc and post the
appropriate messages
under the param of
WM_USER
**************/
void CALLBACK TimerProc(
	UINT	wTimerID,
	UINT	msg,
	DWORD	dwUser,
	DWORD	dw1,
	DWORD	dw2)
{
	HWND	hwnd;

	hwnd=(HWND)dwUser;

	PostMessage(
		hwnd,					//hwnd
		WM_USER,				//iMsg
		TIMER_UPDATE_FREQUENCY,	//wParam
		0);						//lParam
}

/***************
initializes the
timer(s) to the
frequency chosen.
such a nice happy
timer
***************/
void TimerSet(
	long	*pTimerNumber,
	HWND	hwnd)
{
	*pTimerNumber=	timeSetEvent(
		TIMER_UPDATE_FREQUENCY,	//time
		0,						//accuracy +-0mil
		TimerProc,				//proc to call
		(DWORD)hwnd,			//data pass
		TIME_PERIODIC);			//once or multiple
}