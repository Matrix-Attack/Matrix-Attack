/****************
tim berry
creation date:11-8-99
****************/
#include <string.h>
#include <stdio.h>
#include <time.h>
//#include "database.h"

//constructor
tagPlayerProfile::tagPlayerProfile()
{
	//
	//initialize this class to 0
	memset(this,0,sizeof(class tagPlayerProfile));
}

/***************
this will initialize
a player profile's
statistics.  only
needs to be called
once upon creation
***************/
void tagVitalStatistics::InitializeStats(
	char	*pName)
{
	FILE	*fp;
	char	FileName[MAX_FILE_LENGTH]=	CHARACTER_DIRECTORY;

	strcat(FileName,pName);

	//
	//open the character file
	fp=fopen(FileName,"rb");

	//no file!  oh shit
	if(!fp)
	{
		//
		//pull off a bluff and zero some memory
		memset(this,0,sizeof(class tagVitalStatistics));
		return;
	}

	//
	//read that funky info in
	fread(this,1,sizeof(class tagVitalStatistics),fp);

	//close
	fclose(fp);
}

/****************
this will create a
character's (non
player) profile.
it sets the stats
and pops teh new
file inside a directory,
this is basically a
tool for the programmer
to create some data
files quick
****************/
void tagVitalStatistics::CreateCharacter(
	char	*pName,
	long	CursorSpd,
	long	FlippingSpd,
	long	AttackRng,
	long	AttackPwr,
	long	DefensivePwr,
	long	SwitchSpd)
{
	FILE	*fp;
	char	FileName[MAX_FILE_LENGTH]=	CHARACTER_DIRECTORY;

	strcat(FileName,pName);

	this->AttackPower=		AttackPwr;
	this->AttackRange=		AttackRng;
	this->CursorSpeed=		CursorSpd;
	this->DefensivePower=	DefensivePwr;
	this->FlippingSpeed=	FlippingSpd;
	this->SwitchingSpeed=	SwitchSpd;

	fp=fopen(FileName,"wb");

	fwrite(this,1,sizeof(class tagVitalStatistics),fp);

	fclose(fp);
}
