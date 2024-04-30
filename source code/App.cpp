
#include <ddraw.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <dplay.h>
#include <dplobby.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "gamemenu.h"


extern APP App;
extern pSURFACERECORD	pSurfRecordHead;
extern pCALLOC_MEMRECORD	pMemRecordHead;
extern GAME_DATA	GameData;

tagApp::tagApp()
{
	ZeroMemory(this, sizeof (APP) );
}

float tagApp::DegToRad(float angle)
{
	return (angle / 180.0f) * PI;
}

tagApp::~tagApp()
{
	if(pDP)
		delete pDP;

}

void tagApp::InitTrig()
{
	float	*pCos	= Cos;
	float	*pSin	= Sin;
	float	angle	= 0;
	int		i		= 0;

	while(i < 360)
	{
		pCos[i]	=  cosf(DegToRad(angle));
		pSin[i]	=  sinf(DegToRad(angle));
		angle	+= 1;
		i++;
	}
	pCos[0]		= 1;
	pCos[90]	= 0;
	pCos[180]	= -1;
	pCos[270]	= 0;

	pSin[0]		= 0;
	pSin[90]	= 1;
	pSin[180]	= 0;
	pSin[270]	= -1;
}