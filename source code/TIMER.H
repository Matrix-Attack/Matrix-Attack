#ifndef TIMER_H
#define TIMER_H
#pragma once

#ifndef TYPES_H
#include "./types.h"
#endif

#define QUARTER_SECOND		0.001f

//*************************************************
//************** TIMER UTILITIES ******************
//*************************************************

enum ETimer_Command {	eTimer_Reset,			// - to reset the timer
						eTimer_Start,			// - to start the timer
						eTimer_Stop,			// - to stop (or pause) the timer
						eTimer_Advance,			// - to advance the timer by 0.1 seconds
						eTimer_GetAbsoluteTime,	// - to get the absolute system time
						eTimer_GetAppTime,		// - to get the current time
						eTimer_GetElapsedTime	// - to get the time that elapsed between eTimer_GetElapsedTime calls
					};

float __stdcall Util_Timer( ETimer_Command command );

class CTimer
{
private:	
	f32		m_fLastTime;
	u32		m_iNumFrames;

    u32		m_bFrameMoving;
    u32		m_bSingleStep;

	f32		m_fTime;             // Current time in seconds
	f32		m_fElapsedTime;      // Time elapsed since last frame

	bool	m_bOneSecondPassed;
	bool	m_bQuarterSecondPassed;

	f32		m_fLastOneSecondTime;
	f32		m_fLastQuarterSecondTime;

public:
	f32		m_fFPS;
	
			CTimer()				{ Clear();	}
			~CTimer()				{ Destroy();}
	void	Clear					( void );
	u32		Init					( void );
	u32		Destroy					( void );

	void	StartTime				( void );
	void	StopTime				( void );

	f32		GetTheTime				( void );	//could not call it GetCurrentTime() ugh.
	f32		GetEllapsedTime			( void );	//returns time ellapsed from last time this func was called.
	void	Toggle					( void );	//toggles the timer on and off
	void	Reset					( void );	//reset the timer
	
	bool	IsOneSecondPassed		( void )	{ return m_bOneSecondPassed;		}
	bool	IsQuarterSecondPassed	( void )	{ return m_bQuarterSecondPassed;	}
	bool	UpdateOneSecondTimer	( void );
	bool	UpdateQuarterSecondTimer( void );

	u32		PreDraw					( void );
	u32		PostDraw				( void );
	
	
/*	friend  u32		Init			( void ) ;
	friend  u32		Destroy			( void ) ;
	friend  void	StartTime		( void ) ;
	friend  void	StopTime		( void ) ;
	friend  f32		GetTheTime		( void ) ;	//could not call it GetCurrentTime() ugh.
	friend  void	Toggle			( void ) ;	//toggles the timer on and off
	friend  void	Reset			( void ) ;	//reset the timer
	friend  u32		PreDraw			( void ) ;
	friend  u32		PostDraw		( void ) ;
*/
};
/*
static	CTimer	g_timer;
namespace Timer
{
	inline u32	Init		( void ) { return g_timer.Init();		}
	inline u32	Destroy		( void ) { return g_timer.Destroy();	}
	inline void	StartTime	( void ) {		  g_timer.StartTime();	}
	inline void	StopTime	( void ) {		  g_timer.StopTime();	}
	inline f32	GetTheTime	( void ) { return g_timer.GetTheTime(); }	//could not call it GetCurrentTime() ugh.
	inline void	Toggle		( void ) {		  g_timer.Toggle();		}	//toggles the timer on and off
	inline void	Reset		( void ) {		  g_timer.Reset();		}	//reset the timer
	inline u32	PreDraw		( void ) { return g_timer.PreDraw();	}
	inline u32	PostDraw	( void ) { return g_timer.PostDraw();	}
}
*/

#endif