//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/18/01
//	DESCRIPTION:	SOUNDFILE.CPP		
//**********************************************************************************
#include <windows.h>
#include <string>
#include <fstream>

using namespace std;

//#include "./mmgr.h"

#include "fmod.h"
#include "fmod_errors.h"
#include "gsound.h"
#include "utility.h"


#include "./types.h"
#include "./win_utilities.h"




//**********************************************************************************
//	FUNCTION NAME:	CSound::ParseSoundFile
//	AUTHOR(S):		Elijah Emerson
//	CREATION DATE:	7/25/2001
//	
//	ARGUMENT 1:		char* szFile	- filename of where to find sounds and theire indexes.
//	RETURN TYPE:	int 			- 1 == success, 0 == fail
//	DESCRIPTION:	New ParseSoundFile function that will look into the Music Directory
//					for *ANY* MP3 file and put it into the Track list.
//**********************************************************************************
int CSound::ParseSoundFile( char* szFile )
{
	FILE*		fp;
	char		str[256];
	char		buffer[256];
	int			iCurIndex;

	fp = fopen(szFile, "rt");

	if(!fp)
	{
		return 0;
	}

	// Read until end of file
	while(!feof(fp))
	{
		// Keep looking for characters until we find one
		if( File_GetString_NoQuotes(fp, str) )
			break;

		// If this is a comment move to the next line
		if(str[0] == '#')
		{
			File_GetString_NoQuotes(fp, buffer );
			continue;
		}
		else if( !strcmp(str, "NUMSOUNDEFFECTS") )
		{
			File_GetInt(fp, &m_iNumStaticSounds );

			if(m_iNumStaticSounds > 0)
				m_cStaticSoundList = new CSoundSample[m_iNumStaticSounds];
		}
		else if( !strcmp(str, "STATICINDEX") )
		{
			File_GetInt(fp, &iCurIndex );
		}
		else if( !strcmp(str, "STATICNAME") )
		{
			//fp >> m_cStaticSoundList[iCurIndex].m_strName;
			File_GetString_NoQuotes(fp, buffer );
			m_cStaticSoundList[iCurIndex].m_strFileName = buffer;
		}
		else
		{
			MessageBox(NULL, "Invalid entry in \"sound.txt\" file.", 
				"STARTUP ERROR", MB_OK | MB_ICONEXCLAMATION );
		}
	}

	// clean up
	fclose( fp );
	

	// Ok Im going ot re-write this bad boy so we can drag and drop files
	// into the music folder and randomly select one each time we go to play
	// a song. ( which means loading the music on the fly, not all at startup )
	CFileList	FileListMP3;
	FileListMP3.LoadLists( "Music\\*.mp3", false);
	
	//The number of mp3's in the music folder is the number of songs we can use
	m_iNumSongs = FileListMP3.GetNumFiles();
	
	if( m_iNumSongs == 0 )
	{
		MessageBox(NULL, "Could not find any music in the Music folder!", 
			"STARTUP ERROR", MB_OK | MB_ICONEXCLAMATION );
	}
	else
	{
		//Allocate memory for all the songs needed
		m_cTrackList = new CSoundSample[m_iNumSongs];
		
		if( m_cTrackList == NULL )
		{
			MessageBox(NULL, "Could not allocate enough memory for Music.\n(try taking out some mp3 files in the music directory)", 
				"STARTUP ERROR", MB_OK | MB_ICONEXCLAMATION );
			return 0; //error code
		}
		
		for(int i=0; i < m_iNumSongs; i++)
		{
			//The filename of the mp3 will be the track name as well
			if( strlen( FileListMP3.GetFileName(i) ) )
			{
				m_cTrackList[i].m_strFileName = FileListMP3.GetPathAndFileName(i);
				m_cTrackList[i].m_strSongName = FileListMP3.GetFileName(i);
			}
			else
			{
				m_cTrackList[i].m_strFileName = "undefined";
				m_cTrackList[i].m_strSongName = "undefined";
			}
		}
	}
	//CleanUp
	FileListMP3.Destroy();

	return 1; //success code
}


/*
//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/29/01
//	FUNCTION NAME:	CSound::ParseSoundFile
//	RETURN TYPE:	int 
//	ARGUMENT 1:		char* szFile
//	DESCRIPTION:			
//**********************************************************************************
int CSound::ParseSoundFile(char* szFile)
{
	ifstream	inFile(szFile);
	string		str;
	char		cFirst = ' ';
	char		szJunk[100];
	int			iCurIndex;

	if(!inFile)
	{
		return 0;
	}

	// Read until end of file
	while(!inFile.eof())
	{
		// Keep looking for characters until we find one
		while( (cFirst < 'a' || cFirst > 'z') && (cFirst < 'A' || cFirst > 'Z') 
			  && (cFirst != '#') && (!inFile.eof()) )
			  inFile.get(cFirst);
 
		if(inFile.eof() )
			break;

		inFile.seekg(-(sizeof(char)), ios::cur);
		inFile >> str;

		// If this is a comment move to the next line
		if(str[0] == '#')
		{
			int iC;

			inFile.getline(szJunk, 100);

			iC = inFile.gcount();
			cFirst = ' ';
			continue;
		}
		// Otherwise get the properties specific to the object
		else if(str == "NUMMUSICTRACKS")
		{
			if(!GoToNumber(inFile) )
			{
				inFile.close();
				return 0;
			}

			inFile >> m_iNumSongs;

			// Allocate the space for sound samples
			if(m_iNumSongs > 0)
				m_cTrackList = new CSoundSample[m_iNumSongs];

			cFirst = ' ';
		}
		else if(str == "NUMSOUNDEFFECTS")
		{
			if(!GoToNumber(inFile) )
			{
				inFile.close();
				return 0;
			}

			inFile >> m_iNumStaticSounds;

			if(m_iNumStaticSounds > 0)
				m_cStaticSoundList = new CSoundSample[m_iNumStaticSounds];

			cFirst = ' ';
		}
		else if(str == "TRACKINDEX")
		{
			if(!GoToNumber(inFile) )
			{
				inFile.close();
				return 0;
			}

			inFile >> iCurIndex;

			cFirst = ' ';
		}
		else if(str == "STATICINDEX")
		{
			if(!GoToNumber(inFile) )
			{
				inFile.close();
				return 0;
			}

			inFile >> iCurIndex;

			cFirst = ' ';
		}
		else if(str == "TRACKNAME")
		{
			inFile >> m_cTrackList[iCurIndex].m_strName;

			cFirst = ' ';
		}
		else if(str == "STATICNAME")
		{
			inFile >> m_cStaticSoundList[iCurIndex].m_strName;

			cFirst = ' ';
		}
		else
			;
		//	cout << "Invalid entry in text file" << endl;

	}

	inFile.close();

	return 1;
}
*/



/*
#include "win_utilities.h"
int CSound::ParseSoundFile(char* szFile)
{
	FILE*		fp;
	char		str[256];
	char		buffer[256];
	int			iCurIndex;

	fp = fopen(szFile, "rt");

	if(!fp)
	{
		return 0;
	}

	// Read until end of file
	while(!feof(fp))
	{
		// Keep looking for characters until we find one
		if( File_GetString_NoQuotes(fp, str) )
			break;

		// If this is a comment move to the next line
		if(str[0] == '#')
		{
			File_GetString_NoQuotes(fp, buffer );
			continue;
		}
		// Otherwise get the properties specific to the object
		else if( !strcmp(str, "NUMMUSICTRACKS") )
		{		
			//fp >> m_iNumSongs;
			File_GetInt(fp, &m_iNumSongs );

			// Allocate the space for sound samples
			if(m_iNumSongs > 0)
				m_cTrackList = new CSoundSample[m_iNumSongs];
		}
		else if( !strcmp(str, "NUMSOUNDEFFECTS") )
		{
			File_GetInt(fp, &m_iNumStaticSounds );

			if(m_iNumStaticSounds > 0)
				m_cStaticSoundList = new CSoundSample[m_iNumStaticSounds];
		}
		else if( !strcmp(str, "TRACKINDEX") )
		{
			File_GetInt(fp, &iCurIndex );
		}
		else if( !strcmp(str, "STATICINDEX") )
		{
			File_GetInt(fp, &iCurIndex );
		}
		else if( !strcmp(str, "TRACKNAME") )
		{
			//fp >> m_cTrackList[iCurIndex].m_strName;
			File_GetString_NoQuotes(fp, buffer );
			m_cTrackList[iCurIndex].m_strName = buffer;
		}
		else if( !strcmp(str, "STATICNAME") )
		{
			//fp >> m_cStaticSoundList[iCurIndex].m_strName;
			File_GetString_NoQuotes(fp, buffer );
			m_cStaticSoundList[iCurIndex].m_strName = buffer;
		}
		else
			;
		//	cout << "Invalid entry in text file" << endl;
	}
	
	fclose( fp );
	
	return 1;
}
*/