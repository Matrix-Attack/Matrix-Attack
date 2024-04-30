#include <stdlib.h>	//for qsort()
#include <string.h>
#include <stdio.h>

#include ".\mmgr.h"
#include "database.h"



void strToupper( char* string )
{
	int len = strlen(string);
	while(len--)
		string[len] = toupper( string[len] );
}

//*******************************************
//******* Functions for class CRecord *******
//*******************************************
void CRecord::Clear( void )
{
	// High Scores
	m_iTotalScore		= 0;	// 
	m_fLongestStreak	= 0;	// longest time board was *active*
	
	m_iTotalCombos		= 0;	//
	m_iHighestCombo		= 0;	// 
	
	m_iTotalLinkers		= 0;	//
	m_iHighestLinker	= 0;	// 
	
	m_iTotalClusters	= 0;	//
	m_iHighestCluster	= 0;	//

}

void CRecord::ClearScore( void )
{
	// High Scores
	m_iTotalScore		= 0;	// 
	m_fLongestStreak	= 0;	// longest time board was *active*
	
	m_iTotalCombos		= 0;	//
	m_iHighestCombo		= 0;	// 
	
	m_iTotalLinkers		= 0;	//
	m_iHighestLinker	= 0;	// 
	
	m_iTotalClusters	= 0;	//
	m_iHighestCluster	= 0;	//

}
//********************************************
//******* Functions for class CProfile *******
//********************************************

void CProfile::Clear( void )
{
	// Profile info
	m_strName[0]		= NULL;
	
	m_iCharacterID		= 0;	//0-fox,1-vine,2-ninja,3-bundy
	m_iTotalGamesPlayed	= 0;
	
	m_bEmpty			= true;
	
	m_BestRecord.Clear();
	m_CurrentRecord.Clear();
}

void CProfile::Destroy( void )
{
	Clear();
}

void CProfile::Init( char* strName, u32 iCharacterID )
{
	Clear();

	m_bEmpty = false;

	if(strName[0] != 0)
		strcpy(m_strName, strName);
	else
		strcpy(m_strName, "No Name");
	
	m_iCharacterID = iCharacterID;
}

void CProfile::ClearScore( void )
{
	
	m_CurrentRecord.ClearScore();
}
//********************************************
//******* Functions for class CDataBase ******
//********************************************

void CDataBase::Clear( void )
{
	m_iCurGameType		= 0;	// 0 == TimeTrial, 1 == Versus
	m_pActiveProfile	= NULL;
	m_iActiveSlot		= 0;
	
	m_iNumProfiles		= 0;
	m_iNumRecords		= 0;
}

void CDataBase::Destroy( void )
{

}

void CDataBase::Init( void )
{
	Clear();

	if( !LoadDatabase( DATABASE_FILENAME ) )
	{
		//error loading database
	}
}

//**********************************************************************************
//	AUTHOR(S):		Eli Emerson
//	DATE:			5/9/2001
//	FUNCTION NAME:	CDataBase::CreateProfile
//	RETURN TYPE:	s32					- index of profile 
//	ARGUMENT 1:		char* strName		- name of character
//	ARGUMENT 2:		u32 iCharacterID	- id of character
//	DESCRIPTION:	creates a profile in our database		
//**********************************************************************************
s32 CDataBase::CreateProfile( char* strName, u32 iCharacterID )
{
	s32 i;
	
	if( m_iNumProfiles+1 >= MAX_PROFILES )
	{
		return -1;//our database is full!
	}
	
	//cuz our font only supports uppercase... (sucky)
	strToupper( strName );

	//first make sure its not a dupilcate name
	for(i=0; i < MAX_PROFILES; i++)
	{
		if( !strcmp(m_ProfileArray[i].m_strName, strName) )
			return -1;//found a duplicate profile... this is not allowed.
	}
	
	//find an empty slot
	for(i=0; i < MAX_PROFILES; i++)
	{
		if( m_ProfileArray[i].m_bEmpty == true )
		{
			//found an empty slot
			m_ProfileArray[i].Init( strName, iCharacterID );
			m_iNumProfiles++;
			return i;
		}
	}
	
	return -1;
}

//**********************************************************************************
//	AUTHOR(S):		Eli Emerson
//	DATE:			5/9/2001
//	FUNCTION NAME:	CDataBase::DeleteActiveProfile
//	RETURN TYPE:	bool	- true == success, false == failure
//	ARGUMENT 1:		void
//	DESCRIPTION:	deletes the active profile
//**********************************************************************************
bool CDataBase::DeleteProfile( u32 iProfileSlot )
{
	if( m_ProfileArray[iProfileSlot].m_bEmpty == false )
	{
		if(m_pActiveProfile == m_ProfileArray + iProfileSlot)
		{
			m_iActiveSlot	 = 0;
			m_pActiveProfile = NULL;
		}
		
		//found a slot with a profile to delete
		m_ProfileArray[iProfileSlot].Destroy();
		m_iNumProfiles--;
		
		return true;
	}
	
	return false;
}

//**********************************************************************************
//	AUTHOR(S):		Eli Emerson
//	DATE:			5/9/2001
//	FUNCTION NAME:	CDataBase::SetActiveProfile
//	RETURN TYPE:	bool			- false == not found, true == found
//	ARGUMENT 1:		char strName	- name of character's profile to look for
//	DESCRIPTION:	Activates a profile for updating of scores and all that goodness.
//**********************************************************************************
bool CDataBase::SetActiveProfile( char* strName )
{	
	// Search for the wanted profile
	for(u32 i=0; i < MAX_PROFILES; i++)
	{
		if( !m_ProfileArray[i].m_bEmpty &&
			!strcmp(strName, m_ProfileArray[i].m_strName) )
		{
			//Found our profile.
			m_iActiveSlot		= i;
			m_pActiveProfile	= m_ProfileArray + i;
			
			//make sure our current record has the correct info about our profile
			m_ProfileArray[i].ClearScore();
			m_ProfileArray[i].m_CurrentRecord.Clear();
			strcpy( m_ProfileArray[i].m_CurrentRecord.m_strName, strName );
			
			return true;
		}
	}

	//could not find our profile!
	return false;
}

bool CDataBase::SetActiveProfile( u32 iSlot )
{
	if( !m_ProfileArray[iSlot].m_bEmpty )
	{
		m_iActiveSlot	 = iSlot;
		m_pActiveProfile = m_ProfileArray + iSlot;
			
		//make sure our current record has the correct info about our profile
		m_ProfileArray[iSlot].ClearScore();
		m_ProfileArray[iSlot].m_CurrentRecord.Clear();

		return true;
	}
	
	return false;
}

//**********************************************************************************
//	AUTHOR(S):		Eli Emerson
//	DATE:			5/9/2001
//	FUNCTION NAME:	CDataBase::LoadDatabase
//	RETURN TYPE:	bool	- false == could not load, true == 
//	ARGUMENT 1:		void
//	DESCRIPTION:	Loads our database
//**********************************************************************************
bool CDataBase::LoadDatabase( char* strFileName )
{
	FILE*	fp;
	
	fp = fopen( strFileName, "rb");
	
	if(!fp)
	{
		//this is a brand new database!!! 
		return false;
	}

	//we found our database, lets load it!
	fread( this, sizeof(CDataBase), 1, fp);
	
	fclose(fp);

	return true;
}

//**********************************************************************************
//	AUTHOR(S):		Eli Emerson
//	DATE:			5/9/2001
//	FUNCTION NAME:	CDataBase::SaveDatabase
//	RETURN TYPE:	bool	- false == could not save, true == 
//	ARGUMENT 1:		void
//	DESCRIPTION:	Saves our database
//**********************************************************************************
bool CDataBase::SaveDatabase( char* strFileName )
{
	FILE*	fp;
	
	fp = fopen( strFileName, "wb");
	
	if(!fp) // could not save our database
		return false;

	fwrite( this, sizeof(CDataBase), 1, fp);

	fclose(fp);

	return true;
}

// sort the array by score
int CompareRecords( const void *arg1, const void *arg2 )
{
	if( ((CRecord*)arg1)->m_iTotalScore < ((CRecord*)arg2)->m_iTotalScore )
		return 1;
	else if( ((CRecord*)arg1)->m_iTotalScore > ((CRecord*)arg2)->m_iTotalScore )
		return -1;
	return 0;
}

// Check Records after every game.
bool CDataBase::CheckRecords( void )
{
	if(!m_pActiveProfile)
		return false;

	//a game just ended... lets see if it broke any records.
	
	// How the HI-SCORE table works programmah style
	// 1.) put our current record in the array (no matter what it is).
	// 2.) sort the array by score.
	// 3.) Save the new records
	
	// Note: when you display the records never show the last record in the array.

	// 1.) put our record in the array
	m_RecordArray[MAX_RECORDS-1] = m_pActiveProfile->m_CurrentRecord;

	// 2.) sort the array by score
	qsort(m_RecordArray, MAX_RECORDS, sizeof(CRecord), CompareRecords );
	
	// 3.) see if our current record is better than our BEST record
	if( CompareRecords(&m_pActiveProfile->m_CurrentRecord, &m_pActiveProfile->m_BestRecord) )
	{
		m_pActiveProfile->m_BestRecord = m_pActiveProfile->m_CurrentRecord;
	}
	
	// 4.) Up the number of games played counter
	m_pActiveProfile->m_iTotalGamesPlayed++;

	// 5.) Reset our current record
	m_pActiveProfile->ClearScore();
	strcpy(m_pActiveProfile->m_CurrentRecord.m_strName, m_pActiveProfile->GetName() );
	
	// 6.) Finnaly save our DataBase
	if( false == SaveDatabase( DATABASE_FILENAME ) )
	{
		return false;
	}

	return true;
}
	
// Update Active profile functions.
bool CDataBase::UpdateScore( u32 iNewPoints )
{
	if(m_pActiveProfile)
		return m_pActiveProfile->m_CurrentRecord.CheckScore( iNewPoints );	
	return false;
}

bool CDataBase::UpdateStreak( f32 fStreakTime )
{
	if(m_pActiveProfile)
		return m_pActiveProfile->m_CurrentRecord.CheckStreak( fStreakTime );
	return false;
}

bool CDataBase::UpdateCombo( u32 iComboCount )
{
	if(m_pActiveProfile)
		return m_pActiveProfile->m_CurrentRecord.CheckCombo( iComboCount );
	return false;
}

bool CDataBase::UpdateLinker( u32 iLinkerCount )
{
	if(m_pActiveProfile)
		return m_pActiveProfile->m_CurrentRecord.CheckLinker( iLinkerCount );
	return false;
}

bool CDataBase::UpdateCluster( u32 iClusterCount )
{
	if(m_pActiveProfile)
		return m_pActiveProfile->m_CurrentRecord.CheckCluster( iClusterCount );
	return false;
}

