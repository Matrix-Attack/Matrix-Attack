// Utility.h
#ifndef UTILITY_H
#define UTILITY_H


void CreateDirIfNonExistant(char* szDirectory);
void CreateDirIfNonExistant(char* szPathname, char* szDirectory);
void LogErrorToFile(char *szErrorMsg = "Unspecified error ", char *szComment = "Specify an error",
					char *szFilename = NULL, char *szDirectory = "Errors\\");
int GoToNumber(ifstream& ifFile);

#endif