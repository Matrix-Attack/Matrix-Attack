#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace std;
#include "./mmgr.h"

#include "utility.h"




//***********************************************************************
// Function: GoToNumber
//
// Purpose:  Skips ahead in the current file stream until a number
//			 is found.		 
//
// Parameters: ifstream& ifFile - Current file stream
//			  
// Returns: int - 1 for success, 0 otherwise 
//
// Last Modified:  Date 10/03/00      Author - Max Szlagor     
//***********************************************************************
int GoToNumber(ifstream& ifFile)
{
	char cur = ' ';

	while( (cur < '0' || cur > '9') && cur != '-')
	{
		ifFile.get(cur);
		if(ifFile.eof() )
			return 0;
	}

	ifFile.seekg(-(sizeof(char)), ios::cur);

	return 1;
}

//***********************************************************************
// Function: CreateDirIfNonExistant - version 1
//
// Purpose:  Creates a directory in the current directory if the specified
//           directory does not yet exist.
//
// Parameters: char* Directory - The directory to create
//			   			  
//
// Returns: void
//
// Last Modified:  Date 10/15/99      Author - Max Szlagor     
//
//***********************************************************************
void CreateDirIfNonExistant(char* szDirectory)
{
   // First check to see if we have an errors folder, if not we must
   // create the directory
   if(!SearchPath(NULL, szDirectory, NULL, NULL, NULL, NULL))
	    CreateDirectory(szDirectory, NULL);
}



//***********************************************************************
// Function: CreateDirIfNonExistant - version 2
//
// Purpose:  Creates a directory in the specified path if that directory 
//			 does not yet exist.  If no path is specified then the specified
//           directory is created in the current directory
//
// Parameters: char* Directory - The directory to create
//			   char* pathname - The name of the path to put the directory in			   			  
//
//
// Returns: void
//
// Last Modified:  Date 10/15/99      Author - Max Szlagor     
//
//***********************************************************************
void CreateDirIfNonExistant(char* szPathname, char* szDirectory)
{
	char Dir[200] = "";

   // First check to see if we have an errors folder, if not we must
   // create the directory
   if(!SearchPath(szPathname, szDirectory, NULL, NULL, NULL, NULL))
   {
	  if(!szPathname)
	  {
		strcat(Dir, ".\\");
		strcat(Dir, szDirectory);
	    CreateDirectory(Dir, NULL);
	  }
	  else
	  {
		strcat(Dir, szPathname);
		strcat(Dir, szDirectory);
		CreateDirectory(Dir, NULL);
	  }	
   }
}


//***********************************************************************
// Function: LogErrorToFile
//
// Purpose:  Creates a file with either the specified name or the current date 
//			 for the name if no name is given.
//			 Puts this file in the specified directory or a default one and
//			 logs the time the error occurred as well as any additional
//			 error info and comments the user wants to put in.
//          
//
// Parameters: char* Directory  - The directory where the file should go
//			   char* szFileName - The name of the error file
//			   char* szErrorMsg - The error message to be logged
//			   char* szComment  - An additional user specified comment			   			  
//
//
// Returns: void
//
// Last Modified:  Date 10/15/99      Author - Max Szlagor     
//
//***********************************************************************
void LogErrorToFile(char *szErrorMsg, char *szComment,
					char *szFilename, char *szDirectory)
{
	FILE	   *fp;
	struct tm  *ltime;
	time_t     ttime;
	char	   szTime[100];
	char       szName[100] = "";

   // Get the time and convert it to local time, then make it a string with asctime
   time(&ttime);                
   ltime = localtime(&ttime);  
   strcpy(szTime, asctime(ltime) );


   // Print local time as a string 
   strcat(szName, szDirectory);
   if(!szFilename)
        strncat(szName, szTime, 10);
   else
        strcat(szName, szFilename);
   

   // Make the file a text file by default and open it in append mode
   strcat(szName, ".txt");
   fp = fopen(szName, "a");

   if(!fp)
	 return;

   // asctime converts the local time to an ascii string which can be output
   fprintf(fp, "\nError: %s\nComment: %s\n%s", szErrorMsg, szComment, asctime(ltime) ); 
	
   fclose(fp);
}




