/****************
tim berry
creation date:11-8-99
****************/
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <io.h>
#include <windows.h>
#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include "DPlay.h"
//#include "database.h"
#include "app.h"

extern APP			App;
extern GAME_DATA	GameData;

tagDataBase::tagDataBase()
{
	memset(this,0,sizeof(tagDataBase));
}

/******************
this places a players
name inside the player
profile list.  also
accompanying the name
is a handy little
timestamp
******************/
void tagDataBase::AddPlayerProfile(
	tagPlayerProfile	*pProfile)
{
	FILE	*fpWrite = NULL;
	char	FileRead[MAX_FILE_LENGTH]=	"",
			FileWrite[MAX_FILE_LENGTH]=	"";
	time_t	ttime;
	tm		*ltime;

	//
	//get path of players names
	this->AquireReadWritePaths(
		FileRead,
		FileWrite,
		PROFILE_DIRECTORY,
		PROFILE_LIST);

	//
	//open for read and write, adding player to profile list
	fpWrite=fopen(FileWrite,"r+");

	if(!fpWrite)
	{
		MessageBox(NULL,"Could not open file to write profile","Error",0);
		return;
	}


	fseek(fpWrite,0,SEEK_END);

	time(&ttime);
	ltime=localtime(&ttime);

	//
	//print the user name and time stamp it
	fprintf(fpWrite,"\n%s\t%.3d%.2d",
		pProfile->UserID,ltime->tm_year,ltime->tm_mon);

	fclose(fpWrite);
}

/*****************
this is called when
a player loads their
character.  this
will renew the
timestamp put upon
the player data list.
preventing the character
from time deletion
*****************/
void tagDataBase::RenewTimestamp(
	tagPlayerProfile	*pProfile)
{
	FILE	*fp = NULL;
	char	FileName[MAX_FILE_LENGTH]=	PROFILE_DIRECTORY,
			Name[MAX_PROFILE_NAME]=		"";

	time_t	ttime;
	tm		*ltime;

	long	timeStamp;

	time(&ttime);
	ltime=localtime(&ttime);

	strcat(FileName,PROFILE_LIST);

	fp=fopen(FileName,"r+");

	if(!fp)
	{
		MessageBox(NULL,"Could not open file to write timestamp", "Error",0);
		return;
	}

	//
	//run through file looking for players name
	while(fscanf(fp,"%s",Name) != EOF)
	{
		//
		//if we have found the players name
		if(strcmp(Name,pProfile->UserID) == 0)
		{
			fseek(fp,1,SEEK_CUR);

			fprintf(fp,"%.3d%.2d",ltime->tm_year,ltime->tm_mon);
			break;
		}

		//
		//get past the timestamp in the file
		if(fscanf(fp,"%d",&timeStamp) == EOF)
			break;
	}

	fclose(fp);
}

/****************
this allows me to
find out which file
to read from and
which to write to
****************/
void tagDataBase::AquireReadWritePaths(
	char	FileRead[],
	char	FileWrite[],
	char	Directory[],
	char	File[])
{
	FILE	*fp = NULL;
	char	FileName[MAX_FILE_LENGTH]=	"";

	strcat(FileName,Directory);
	strcat(FileName,File);

	//
	//open the temp data file to find which data file i
	//should read to and which t write to
	fp=fopen(FileName,"r");

	if(!fp)
	{
		MessageBox(NULL,"Could not open file to read AquireReadWritePaths", "Error",0);
		return;
	}

	//
	//read in the data file to read and write to
	fscanf(fp,"%s",FileRead);
	fscanf(fp,"%s",FileWrite);

	fclose(fp);

	//
	//reopen file and swap values of read and write
	fp=fopen(FileName,"w");

	if(!fp)
	{
		MessageBox(NULL,"Could not open file to write AquireReadWritePaths", "Error",0);
		return;
	}

	fprintf(fp,"%s\n",FileWrite);
	fprintf(fp,"%s",FileRead);

	fclose(fp);
}

/*****************
kind of crappy, but
what do you expect
from a first attempt
at encryption.  this
takes a userid and
password and a pointer
to a hashe table to
fill with data
*****************/
void tagDataBase::Encrypt(
	char	*pUserID,
	char	*pPassword,
	short	Hashe[])
{
	long	stringLength;
	long	counter,
			subCounter,
			idIndex;
	long	value;

	//
	//set the hashe to 0
	memset(Hashe,0,sizeof(short)*HASHE_TABLE_SIZE);

	stringLength=	strlen(pUserID);

	for(counter=0;counter < stringLength;++counter)
	{
		value=	(pPassword[counter]*pPassword[counter]);

		value/=	(value % 10)+1;

		subCounter=0;
		do{
			subCounter++;

			if((unsigned)(subCounter + counter) > strlen(pPassword))
			{
				subCounter=strlen(pPassword) - counter;
				break;
			}

		}while(Hashe[(unsigned)pPassword[counter + subCounter]]);

		Hashe[(unsigned)pPassword[counter + subCounter]]=	(short)value;
	}

	counter=0;
	do{
		counter++;
	}while(Hashe[counter]);
	Hashe[counter]=	(short)stringLength;

	for(idIndex=0;counter < HASHE_TABLE_SIZE;++counter)
	{
		if(!Hashe[counter])
			Hashe[counter]=	(short)(pUserID[idIndex] - counter + idIndex);

		++idIndex;

		if(idIndex > stringLength)
			idIndex=	0;
	}
}

/****************
this will copy one
file to another, if
the file to write
to does not exist
it is created, else
it is destroyed and
overwritten
****************/
void tagDataBase::FileCopy(
	char	*FileNameR,
	char	*FileNameW)
{
	FILE	*fpRead = NULL,
			*fpWrite = NULL;

	char	temp;

	//
	//open files to read and write from
	fpRead=	fopen(FileNameR,"r");
	fpWrite=fopen(FileNameW,"w");
 
	if(!fpRead)
	{
		MessageBox(NULL,"Could not open file to read FileCopy", "Error",0);
		return;
	}

	if(!fpWrite)
	{
		MessageBox(NULL,"Could not open file to write FileCopy", "Error",0);
		return;
	}
	//
	//copy file one char at a time
	while(fscanf(fpRead,"%c",&temp) != EOF)
		fprintf(fpWrite,"%c",temp);

	fclose(fpRead);
	fclose(fpWrite);
}

/*****************
this is called when
a player profile is
being deleted.  this
removes the layers
name from the high
score list
*****************/
void tagDataBase::RemoveHighScore(
	char	*pUserID)
{
	FILE	*fpRead = NULL,
			*fpWrite = NULL;

	char	FileRead[MAX_FILE_LENGTH]=	"",
			FileWrite[MAX_FILE_LENGTH]=	"",
			ProfileID[MAX_FILE_LENGTH]=	"";

	long	highScore;

	//
	//get path names to read and write to
	this->AquireReadWritePaths(
		FileRead,
		FileWrite,
		RANKING_DIRECTORY,
		HIGH_SCORE_FILE);

	//
	//open the files
	fpRead=fopen(FileRead,"r");
	fpWrite=fopen(FileWrite,"w");

	if(!fpRead)
	{
		MessageBox(NULL,"Could not open file to read RemoveHighScore", "Error",0);
		return;
	}

	if(!fpWrite)
	{
		MessageBox(NULL,"Could not open file to write RemoveHighScore", "Error",0);
		return;
	}
	//
	//search through file
	while(fscanf(fpRead,"%s %d",ProfileID,&highScore) != EOF)
	{
		//
		//if these names are not equal
		if(strcmp(pUserID,ProfileID) != 0)
			//
			//reprint values
			fprintf(fpWrite,"%s\t%d\n",ProfileID,highScore);
	}

	fclose(fpRead);
	fclose(fpWrite);
}

/*****************
called when a profile
is being deleted,
removes a players
name from the profile
list
*****************/
void tagDataBase::RemoveProfileName(
	char	*pUserID)
{
	FILE	*fpRead,
			*fpWrite;

	char	FileRead[MAX_FILE_LENGTH]=	"",
			FileWrite[MAX_FILE_LENGTH]=	"",
			ProfileID[MAX_FILE_LENGTH]=	"";

	long	timeStamp;

	this->AquireReadWritePaths(
		FileRead,
		FileWrite,
		PROFILE_DIRECTORY,
		PROFILE_LIST);

	fpRead=fopen(FileRead,"r");
	fpWrite=fopen(FileWrite,"w");

	if(!fpRead)
	{
		MessageBox(NULL,"Could not open file to read RemoveProfileName", "Error",0);
		return;
	}

	if(!fpWrite)
	{
		MessageBox(NULL,"Could not open file to write RemoveProfileName", "Error",0);
		return;
	}
	//
	//search through file
	while(fscanf(fpRead,"%s %d",ProfileID,&timeStamp) != EOF)
	{
		//
		//if these names are not equal
		if(strcmp(pUserID,ProfileID) != 0)
			//
			//reprint values
			fprintf(fpWrite,"%s\t%.5d\n",ProfileID,timeStamp);
	}

	fclose(fpRead);
	fclose(fpWrite);
}

/****************
creates a user
profile for the
database.  this
stores the user
profile and places
the name in a data
file with all names
of every player
****************/
long tagDataBase::CreateProfile(
	tagPlayerProfile	*pProfile,
	char				*pUserID,
	char				*pPassword,
	char				*pCharacterName)
{
	FILE	*fp;
	char	FileName[MAX_FILE_LENGTH]=	PROFILE_DIRECTORY;

	if(!strcmp(pUserID,""))
		return PP_USERID_ALREADY_IN_USE;
	//
	//get the directory needed
	strcat(FileName,pUserID);
	strcat(FileName,PROFILE_EXTENSION);

	//
	//a check to see if the file actually exists
	fp=fopen(FileName,"rb");
	if(fp)
	{
		//
		//close file and return error player name already in use
		fclose(fp);
		return PP_USERID_ALREADY_IN_USE;
	}

	//else continue, all good with the name
	//don't need to close a null pointer

	//
	//init the name of the profile
	strcpy(pProfile->UserID,pUserID);

	//and the password hashe
//	Encrypt(
//		pUserID,
//		pPassword,
//		pProfile->HasheTable);

	//
	//initializing the characters stats with a character profile
	pProfile->VitalStats.InitializeStats(pCharacterName);

	//
	//now save the player in the database
	this->LogProfileOff(
		pProfile);

	//
	//add player to the player profile list
	this->AddPlayerProfile(
		pProfile);

	return 0;
}

/****************
this will load a
player profile
into a player profile
that was passed
into this function
****************/
long tagDataBase::LogProfileOn(
	tagPlayerProfile	*pProfile,
	char				*pUserID)
{
	FILE	*fp;
	char	FileName[MAX_FILE_LENGTH]=	PROFILE_DIRECTORY;

	strcat(FileName,pUserID);
	strcat(FileName,PROFILE_EXTENSION);

	//
	//open file to read binary data
	fp=fopen(FileName,"rb");

	if(!fp)
		return PP_PLAYER_NOT_IN_DATABASE;

	//
	//else read in data for player load
	fread(pProfile,1,sizeof(class tagPlayerProfile),fp);

	//
	//close file
	fclose(fp);

	//
	//update the master player list to a new timestamp for this player
	this->RenewTimestamp(
		pProfile);

	//
	//set flag for player now logged on
	pProfile->Flags|=	LOGGED_ON;

	//
	//save the file so that the logged on flag will be inside their file
	this->UpdateProfile(
		pProfile,
		pProfile->HighScore);

	return 0;
}

/*****************
saves the players
profile to a file
of their user id
name.
*****************/
void tagDataBase::LogProfileOff(
	tagPlayerProfile	*pProfile)
{
	FILE	*fp;
	char	FileName[MAX_FILE_LENGTH]=	PROFILE_DIRECTORY;

	//
	//undo flags
	pProfile->Flags=	0;

	//
	//get the file name to open
	strcat(FileName,pProfile->UserID);
	strcat(FileName,PROFILE_EXTENSION);

	//
	//open file in players name to write binary
	fp=fopen(FileName,"wb");

	if(!fp)
	{
		MessageBox(NULL,"Could not open file to write LogProfileOff", "Error",0);
		return;
	}
	//
	//write the class to the file
	fwrite(pProfile,1,sizeof(class tagPlayerProfile),fp);

	//
	//close the file pointer
	fclose(fp);
}

/*****************
calling this will
delete a profile
from the current
files (not the backup),
however when the
backup files have
been replaced with
more modern versions
the profile will
cease to be
*****************/
void tagDataBase::DeleteProfile(
	tagPlayerProfile	*pProfile)
{
	char	FileName[MAX_FILE_LENGTH]=	PROFILE_DIRECTORY;

	//
	//aqurire the path of the profile
	strcat(FileName,pProfile->UserID);
	strcat(FileName,PROFILE_EXTENSION);

	//
	//execute the command, profile deleted
	remove(FileName);

	//
	//now remove profile from high score list and profile list
	this->RemoveHighScore(
		pProfile->UserID);

	this->RemoveProfileName(
		pProfile->UserID);
}

/*****************
checks to make sure
the players password
is correct, returns a
PP_INCORRECT_PASSWORD
if password is not
the same
*****************/
long tagDataBase::CheckPassword(
	tagPlayerProfile	*pProfile,
	char				*pUserID,
	char				*pPassword)
{
/*	long	counter=0;
	short	hashe[HASHE_TABLE_SIZE];

	//
	//initialize the test hashe table
	Encrypt(
		pUserID,
		pPassword,
		hashe);

	while(counter < HASHE_TABLE_SIZE)
	{
		if(hashe[counter] != pProfile->HasheTable[counter])
			return PP_INCORRECT_PASSWORD;

		++counter;
	}
*/
	return PP_CORRECT_PASSWORD;
}

/*****************
called in order to
change a players
password
*****************/
void tagDataBase::ChangePassword(
	tagPlayerProfile	*pProfile,
	char				*pPassword)
{
/*	Encrypt(
		pProfile->UserID,
		pPassword,
		pProfile->HasheTable);
*/
}

/****************
this will update
the high score page
for all to view
it's most recently
accomplished goals
****************/
void tagDataBase::UpdateHighScore(
	tagPlayerProfile	*pProfile,
	unsigned long		Score)
{
	FILE			*fpRead,
					*fpWrite;

	char			FileRead[MAX_FILE_LENGTH]=	"",
					FileWrite[MAX_FILE_LENGTH]=	"";

	char			Name[MAX_PROFILE_NAME]=		"";
	unsigned long	CurHighScore,
					scoreFound=0;
	long			done=0;

	//
	//check if this score is greater than the players previous high score
	if(Score <= pProfile->HighScore)
		return;

	//
	//set the new high score
	pProfile->HighScore=Score;

	//
	//get which files to read and write
	this->AquireReadWritePaths(
		FileRead,
		FileWrite,
		RANKING_DIRECTORY,
		HIGH_SCORE_FILE);

	//
	//now open both files, read from one to write to another
	fpRead=	fopen(FileRead,"r");
	fpWrite=fopen(FileWrite,"w");

	//
	//now as we read from one file, write to another.
	//must keep checking for either a score less than this high or a name from this
	while(!done)
	{
		//
		//if we reached end of file before finishing
		if(fscanf(fpRead,"%s %d",Name,&CurHighScore)==EOF)
		{
			done=1;

			if(!scoreFound)
				fprintf(
					fpWrite,
					"%s %d %d %d %d %d %d\n",
					pProfile->UserID,
					pProfile->GameStats.TimeTrial.HighScore,
					pProfile->GameStats.TimeTrial.AverageScore,
					pProfile->GameStats.TimeTrial.NumberOfGamesPlayed,
					pProfile->GameStats.TimeTrial.Combo,
					pProfile->GameStats.TimeTrial.Linker,
					pProfile->GameStats.TimeTrial.Cluster);

			break;
		}

		//
		//if we have found where to place this guys name
		if((Score > CurHighScore) && !scoreFound)
		{
			fprintf(
				fpWrite,
				"%s %d %d %d %d %d %d\n",
				pProfile->UserID,
				pProfile->GameStats.TimeTrial.HighScore,
				pProfile->GameStats.TimeTrial.AverageScore,
				pProfile->GameStats.TimeTrial.NumberOfGamesPlayed,
				pProfile->GameStats.TimeTrial.Combo,
				pProfile->GameStats.TimeTrial.Linker,
				pProfile->GameStats.TimeTrial.Cluster);
			scoreFound=1;
		}

		//
		//if it's not her name
		if(strcmp(Name,pProfile->UserID) != 0)
			fprintf(
				fpWrite,
				"%s %d %d %d %d %d %d\n",
				Name,
				CurHighScore,
				pProfile->GameStats.TimeTrial.AverageScore,
				pProfile->GameStats.TimeTrial.NumberOfGamesPlayed,
				pProfile->GameStats.TimeTrial.Combo,
				pProfile->GameStats.TimeTrial.Linker,
				pProfile->GameStats.TimeTrial.Cluster);
	}

	//
	//close both file and have a nice day
	fclose(fpRead);
	fclose(fpWrite);
}

/***************
this will update
a players profile
by saving it to
their file.  it
also will update
the high score list
***************/
void tagDataBase::UpdateProfile(
	tagPlayerProfile	*pProfile,
	unsigned long		Score)
{
	FILE	*fp;
	char	FileName[MAX_FILE_LENGTH]=	PROFILE_DIRECTORY;

	//
	//update their high score
	this->UpdateHighScore(
		pProfile,
		Score);

	//
	//get file name to save to
	strcat(FileName,pProfile->UserID);
	strcat(FileName,PROFILE_EXTENSION);

	//
	//open file of players name
	fp=fopen(FileName,"wb");

	if(!fp)
	{
		MessageBox(NULL,"Could not open file to write UpdateProfile", "Error",0);
		return;
	}
	//
	//write the class to the file
	fwrite(pProfile,1,sizeof(class tagPlayerProfile),fp);

	//
	//close the file pointer
	fclose(fp);
}

/****************
this will backup
all files of the
current database.
it has capabilities
for 5 backups,
easily extened
further
****************/
void tagDataBase::BackupDataBase()
{
	struct _finddata_t	file;

	char				FileNameR[MAX_FILE_LENGTH],
						FileNameW[MAX_FILE_LENGTH],
						FolderNameD[MAX_FILE_LENGTH],
						FolderNameC[MAX_FILE_LENGTH],
						FileExtensions[MAX_FILE_LENGTH];

	char				Folders[6][MAX_FILE_LENGTH]=
							{
							PROFILE_DIRECTORY,
							BACKUP_DIRECTORY1,
							BACKUP_DIRECTORY2,
							BACKUP_DIRECTORY3,
							BACKUP_DIRECTORY4,
							BACKUP_DIRECTORY5
							};

	long				counter;
	long				hSearch;

	for(counter=NUMBER_BACKUP_DIRECTORIES;counter;--counter)
	{
		//
		//get current directory
		memset(FolderNameD,0,sizeof(char)*MAX_FILE_LENGTH);
		strcat(FolderNameD,Folders[counter]);

		//
		//get file search requirements
		memset(FileExtensions,0,sizeof(char)*MAX_FILE_LENGTH);
		strcat(FileExtensions,FolderNameD);
		strcat(FileExtensions,"*.pfl");

		//
		//find all files with the extenion .pfl
		if((hSearch=_findfirst(FileExtensions,&file)) != -1)
		{
			//
			//cycle through files
			do{
				//
				//get the file name
				memset(FileNameR,0,sizeof(char)*MAX_FILE_LENGTH);
				strcat(FileNameR,FolderNameD);
				strcat(FileNameR,file.name);

				//
				//delete the file
				remove(FileNameR);

			}while(!_findnext(hSearch,&file));

			_findclose(hSearch);
		}

		//
		//now find all files in previous directory with .pfl
		memset(FolderNameC,0,sizeof(char)*MAX_FILE_LENGTH);
		strcat(FolderNameC,Folders[counter-1]);

		memset(FileExtensions,0,sizeof(char)*MAX_FILE_LENGTH);
		strcat(FileExtensions,FolderNameC);
		strcat(FileExtensions,"*.pfl");

		if((hSearch=_findfirst(FileExtensions,&file)) != -1)
		{
			do{
				memset(FileNameR,0,sizeof(char)*MAX_FILE_LENGTH);
				memset(FileNameW,0,sizeof(char)*MAX_FILE_LENGTH);
				strcat(FileNameR,FolderNameC);
				strcat(FileNameW,FolderNameD);
				strcat(FileNameR,file.name);
				strcat(FileNameW,file.name);

				//
				//copy file, r to w
				this->FileCopy(FileNameR,FileNameW);

			}while(!_findnext(hSearch,&file));

			_findclose(hSearch);
		}
	}
}

/*****************
checks if a player
is logged on at this
time, uses the PP_
defines for return
values
*****************/
long tagDataBase::CheckIfPlayerOn(
	char	*pUserIDtoCheck)
{
	FILE				*fp;

	tagPlayerProfile	Profile;

	char				FileName[MAX_FILE_LENGTH]=	PROFILE_DIRECTORY;

	long				rType;

	//
	//get path name
	strcat(FileName,pUserIDtoCheck);
	fp=fopen(FileName,"rb");

	if(!fp)
		//
		//player not found in database
		return	PP_PLAYER_NOT_IN_DATABASE;

	//
	//load profile in to check values
	fread(&Profile,1,sizeof(class tagPlayerProfile),fp);

	fclose(fp);

	//
	//check if player logged on
	(Profile.Flags & LOGGED_ON)?
		rType=	PP_PLAYER_LOGGED_ON:
		rType=	PP_PLAYER_LOGGED_OFF;

	return rType;
}

/****************
call this when
shutting down the
database.  this will
mark all players as
logged off
****************/
void tagDataBase::DataBaseShutDown()
{
	struct _finddata_t	file;
	FILE				*fp;

	tagPlayerProfile	Profile;

	char				FileName[MAX_FILE_LENGTH]=	"";
	long				hSearch;

	strcat(FileName,PROFILE_DIRECTORY);
	strcat(FileName,PROFILE_EXTENSION);

	hSearch=_findfirst(FileName,&file);

	if(hSearch	!=	-1)
	{
		do{
			//
			//get the file name to open
			memset(FileName,0,sizeof(char)*MAX_FILE_LENGTH);
			strcat(FileName,PROFILE_DIRECTORY);
			strcat(FileName,file.name);

			//
			//open file read in data and close it
			fp=fopen(FileName,"rb");
			fread(&Profile,1,sizeof(class tagPlayerProfile),fp);
			fclose(fp);

			//
			//reset the flag
			Profile.Flags=	0;

			//
			//open file to write structure to and close
			fp=fopen(FileName,"wb");
			fwrite(&Profile,1,sizeof(class tagPlayerProfile),fp);
			fclose(fp);

		}while(_findnext(hSearch,&file) != -1);

		_findclose(hSearch);
	}
}

void tagDataBase::GetHighScoreDataFromServer()
{
	long	data=	DP_MSG_GET_SERVER_STATS;

	//
	//ask server for data
	App.pDP->lpDP->Send(
		App.pDP->dpid,
		DPID_ALLPLAYERS,
		DPSEND_GUARANTEED,
		&data,
		sizeof(long));
}

void tagDataBase::GetPlayerHighScoreDataFromServer(
	char	*pUserID)
{
	DATA_STRUCT	D;
	D.id=DP_MSG_GET_SCORE_DATA_SINGLE_PLAYER;
	strcpy(D.data,pUserID);

	//
	//ask server for data
	App.pDP->lpDP->Send(
		App.pDP->dpid,
		DPID_ALLPLAYERS,
		DPSEND_GUARANTEED,
		&D,
		sizeof(DATA_STRUCT));
}

void tagDataBase::GetPlayerHighScoreDataFromClient(
	char	*pUserID)
{
	if(!pUserID)
		return;

	GameData.Data=this->LoadProfileText(pUserID);
}

/*******************
this will return a
string containing all
the players and their
scores and some
additional data
*******************/
char *tagDataBase::GetHighScoreData()
{
	FILE		*fp;
	_finddata_t	file;
	long		hSearch;

	long		done=0,
				counter=0,
				count=0;

	char		FileRead[MAX_FILE_LENGTH]=	"";
	char		FileWrite[MAX_FILE_LENGTH]=	"";
	char		*Data=	NULL;

	this->AquireReadWritePaths(
		FileRead,
		FileWrite,
		RANKING_DIRECTORY,
		HIGH_SCORE_FILE);

	hSearch=_findfirst(FileRead,&file);

	if(hSearch != -1)
	{
		//
		//allocate the memory required for the file
		Data=	(char*)malloc(sizeof(char)*file.size);
		memset(Data,0,sizeof(char)*file.size);

		fp=fopen(FileRead,"rb");

		fread(Data,1,file.size-2,fp);

		fclose(fp);

		//
		//to shorten string to send across net
		while(!done)
		{
			if(Data[counter] == '\0')
				break;

			if(Data[counter] == '\n')
				++count;

			++counter;

			if(count == 5)//or max number of high score files
			{
				Data[counter]=	'\0';//terminate the string
				done=1;
			}
		}

		_findclose(hSearch);
	}

	return Data;
}

/******************
this will return a
character string that
contains data pertaining
to one user, the name
of that user is the
name passed in
******************/
char *tagDataBase::LoadProfileText(
	char	*pUserID)
{
	tagPlayerProfile	Profile;
	long				test;
	char				*Data;

	if(!strlen(pUserID))
		return NULL;

	test=this->LogProfileOn(
		&Profile,
		pUserID);

	if(test == PP_PLAYER_NOT_IN_DATABASE)
		return NULL;

	//
	//allocate enough memory to put all data into a string
	Data=	(char*)malloc(
				strlen(pUserID)+1 +	//name+1
				10+1	+			//high score
				10+1	+			//high score
				10+1	+			//high score
				10+1	+			//max combo
				10+1				//max kipe
				);

	memset(Data,0,strlen(pUserID) + 34);

	//
	//now print data to string
	sprintf(
		Data,
		"%s %d %d %d %d %d",
		pUserID,
		Profile.GameStats.TimeTrial.HighScore,
		Profile.GameStats.TimeTrial.AverageScore,
		Profile.GameStats.TimeTrial.Combo,
		Profile.GameStats.TimeTrial.Linker,
		Profile.GameStats.TimeTrial.Cluster);

	this->LogProfileOff(
		&Profile);

	return Data;
}
