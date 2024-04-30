//Includes for Util_Timer
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>	//for timeGetTime

#include "./mmgr.h"
#include "./types.h"
#include "./timer.h"



void CTimer::Clear()
{
	m_bFrameMoving		= 0;	//
    m_bSingleStep		= 0;	//

	m_fTime				= 0.0f;	// Current time in seconds
	m_fElapsedTime		= 0.0f;	// Time elapsed since last frame
	
	m_bOneSecondPassed		= false;
	m_bQuarterSecondPassed	= false;
	m_fLastOneSecondTime	= 0.0f;
	m_fLastQuarterSecondTime= 0.0f;
	m_fFPS					= 0.0f;	// Instanteous frame rate
	
}

u32 CTimer::Destroy()
{
	return ER_SUCCESS;
}

u32 CTimer::Init()
{
	Clear();

	m_bOneSecondPassed		= false;
	m_bQuarterSecondPassed	= false;
	m_fLastOneSecondTime	= 0.0f;
	m_fLastQuarterSecondTime= 0.0f;
	
	m_bFrameMoving = 1;
	
	// Setup the app so it can support single-stepping
	Util_Timer( eTimer_Start );

	return ER_SUCCESS;
}

void CTimer::StartTime( void )
{	
	if( m_bFrameMoving )
		Util_Timer( eTimer_Start );
}

void CTimer::StopTime( void )
{
	if( m_bFrameMoving )
		Util_Timer( eTimer_Stop );
}

f32 CTimer::GetTheTime( void )
{
	m_fTime = Util_Timer( eTimer_GetAppTime );
	return m_fTime;
}

f32 CTimer::GetEllapsedTime( void )
{
	return Util_Timer( eTimer_GetElapsedTime );
}

void CTimer::Toggle()
{
	//Toggle Pause State
	m_bFrameMoving = !m_bFrameMoving;

	Util_Timer( m_bFrameMoving ? eTimer_Start : eTimer_Stop );
}

void CTimer::Reset()
{
	Util_Timer( eTimer_Reset );
}

bool CTimer::UpdateOneSecondTimer( void )
{
	if(m_bOneSecondPassed)
	{
		m_bOneSecondPassed = false;		//make sure this function is true "once" per second
		return true;
	}
	return false;
}

bool CTimer::UpdateQuarterSecondTimer( void )
{
	if(m_bQuarterSecondPassed)
	{
		m_bQuarterSecondPassed = false;
		return true;
	}
	return false;
}

u32 CTimer::PreDraw()
{
	// Get the app's time, in seconds. 
	f32 fGameTime		= Util_Timer( eTimer_GetAppTime );
	
	// FrameMove (animate) the scene
	if( m_bFrameMoving || m_bSingleStep )
	{
		// Store the time for the app
		m_fTime			= fGameTime;
		
		// Frame move the scene
//		if( FAILED( hr = FrameMove() ) )
//			return hr;
		
		m_bSingleStep = 0;
	}

	return ER_SUCCESS;
}

u32 CTimer::PostDraw()
{
	f32 fTime = Util_Timer( eTimer_GetAbsoluteTime );
	
	++m_iNumFrames;
	
	// Update the scene stats once per second
	if( fTime - m_fLastOneSecondTime > 1.0f )
	{
		//Calculate fps
		m_fFPS				= m_iNumFrames / (fTime - m_fLastOneSecondTime);
		m_iNumFrames		= 0;
		
		m_fLastOneSecondTime= fTime;
		m_bOneSecondPassed	= true;
	}
	
	//update our quarter second timer
	if(fTime - m_fLastQuarterSecondTime > QUARTER_SECOND)
	{
		m_fLastQuarterSecondTime = fTime;
		m_bQuarterSecondPassed	 = true;
	}

	return ER_SUCCESS;
}



//-----------------------------------------------------------------------------
// Name: Util_Timer()
// Desc: Performs timer opertations. Use the following commands:
//		  eTimer_Reset			 - to reset the timer
//		  eTimer_Start			 - to start the timer
//		  eTimer_Stop			 - to stop (or pause) the timer
//		  eTimer_Advance		 - to advance the timer by 0.1 seconds
//		  eTimer_GetAbsoluteTime - to get the absolute system time
//		  eTimer_GetAppTime		 - to get the current time
//		  eTimer_GetElapsedTime  - to get the time that elapsed between 
//									eTimer_GetElapsedTime calls
//-----------------------------------------------------------------------------
float __stdcall Util_Timer( ETimer_Command command )
{
	static BOOL	 m_bTimerInitialized	= FALSE;
	static BOOL	 m_bUsingQPF			= FALSE;
	static LONGLONG m_llQPFTicksPerSec  = 0;
	
	// Initialize the timer
	if( FALSE == m_bTimerInitialized )
	{
		m_bTimerInitialized = TRUE;
		
		// Use QueryPerformanceFrequency() to get frequency of timer.  If QPF is
		// not supported, we will timeGetTime() which returns milliseconds.
		LARGE_INTEGER qwTicksPerSec;
		
		m_bUsingQPF = QueryPerformanceFrequency( &qwTicksPerSec );
		if( m_bUsingQPF )
		{
			m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
		}
	}
	
	if( m_bUsingQPF )
	{
		static LONGLONG m_llStopTime		= 0;
		static LONGLONG m_llLastElapsedTime = 0;
		static LONGLONG m_llBaseTime		= 0;

		LARGE_INTEGER	qwTime;
		float			fTime;
		float			fElapsedTime;
		
		// Get either the current time or the stop time, depending
		// on whether we're stopped and what command was sent
		if( m_llStopTime != 0 && command != eTimer_Start && command != eTimer_GetAbsoluteTime)
		{
			qwTime.QuadPart = m_llStopTime;
		}
		else
		{
			QueryPerformanceCounter( &qwTime );
		}
		
		// Return the elapsed time
		if( command == eTimer_GetElapsedTime )
		{
			fElapsedTime = (float) ( qwTime.QuadPart - m_llLastElapsedTime ) / (float) m_llQPFTicksPerSec;
			m_llLastElapsedTime = qwTime.QuadPart;
			return (float) fElapsedTime;
		}
		
		// Return the current time
		if( command == eTimer_GetAppTime )
		{
			float fAppTime = (float) ( qwTime.QuadPart - m_llBaseTime ) / (float) m_llQPFTicksPerSec;
			return (float) fAppTime;
		}
		
		// Reset the timer
		if( command == eTimer_Reset )
		{
			m_llBaseTime		= qwTime.QuadPart;
			m_llLastElapsedTime = qwTime.QuadPart;
			return 0.0f;
		}
		
		// Start the timer
		if( command == eTimer_Start )
		{
			m_llBaseTime		+= qwTime.QuadPart - m_llStopTime;
			m_llStopTime		= 0;
			m_llLastElapsedTime = qwTime.QuadPart;
			return 0.0f;
		}
		
		// Stop the timer
		if( command == eTimer_Stop )
		{
			m_llStopTime		= qwTime.QuadPart;
			m_llLastElapsedTime = qwTime.QuadPart;
			return 0.0f;
		}
		
		// Advance the timer by 1/10th second
		if( command == eTimer_Advance )
		{
			m_llStopTime += m_llQPFTicksPerSec/10;
			return 0.0f;
		}
		
		if( command == eTimer_GetAbsoluteTime )
		{
			fTime = qwTime.QuadPart / (float) m_llQPFTicksPerSec;
			return (float) fTime;
		}
		
		return -1.0f; // Invalid command specified
	}
	else
	{
		// Get the time using timeGetTime()
		static float m_fLastElapsedTime = 0.0;
		static float m_fBaseTime		 = 0.0;
		static float m_fStopTime		 = 0.0;

		float fTime;
		float fElapsedTime;
		
		// Get either the current time or the stop time, depending
		// on whether we're stopped and what command was sent
		if( m_fStopTime != 0.0 && command != eTimer_Start && command != eTimer_GetAbsoluteTime)
		{
			fTime = m_fStopTime;
		}
		else
		{
			fTime = timeGetTime() * 0.001f;
		}
		
		// Return the elapsed time
		if( command == eTimer_GetElapsedTime )
		{   
			fElapsedTime		= (float) (fTime - m_fLastElapsedTime);
			m_fLastElapsedTime	= fTime;
			return (float) fElapsedTime;
		}
		
		// Return the current time
		if( command == eTimer_GetAppTime )
		{
			return (float) (fTime - m_fBaseTime);
		}
		
		// Reset the timer
		if( command == eTimer_Reset )
		{
			m_fBaseTime			= fTime;
			m_fLastElapsedTime  = fTime;
			return 0.0f;
		}
		
		// Start the timer
		if( command == eTimer_Start )
		{
			m_fBaseTime			+= fTime - m_fStopTime;
			m_fStopTime			= 0.0f;
			m_fLastElapsedTime  = fTime;
			return 0.0f;
		}
		
		// Stop the timer
		if( command == eTimer_Stop )
		{
			m_fStopTime = fTime;
			return 0.0f;
		}
		
		// Advance the timer by 1/10th second
		if( command == eTimer_Advance )
		{
			m_fStopTime += 0.1f;
			return 0.0f;
		}
		
		if( command == eTimer_GetAbsoluteTime )
		{
			return (float) fTime;
		}
		
		return -1.0f; // Invalid command specified
	}
}