#include <ddraw.h>
#include <stdio.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "gamemenu.h"
#include "dinput.h"

extern APP			App;
extern GAME_DATA	GameData;


#define	REPEAT_WAIT_TIME	10

void ChatInput(char *Buffer, unsigned int maxChar)
{

//	static int		RepeatTimer = 0; 
	unsigned int	StrLength	= strlen(Buffer);
	U32*			pKeys		= App.pdistruct->curKeyState;

//	if(RepeatTimer)
//	{
//		RepeatTimer--;
//		return;
//	}

	if(GameData.keysDown[VK_BACK])
//	if(pKeys[DIK_BACK] == 1 || (pKeys[DIK_BACK] % 9 == 0 && pKeys[DIK_BACK] > 1))
	{
		if(StrLength)
		{
			if(Buffer[StrLength - 1] >= '0' && Buffer[StrLength - 1] <= '9' && 
			   Buffer[StrLength - 2] == 'C' && Buffer[StrLength - 3] == '%')
			{//deleting the 3 char color flag + 1 regular character
				Buffer[StrLength - 4] = NULL;
			}
			else
				Buffer[StrLength - 1] = NULL;
			GameData.keysDown[VK_BACK] = 0;
		}
//		RepeatTimer = 5;
		return;
	}

	//Must be checked after checking backspace! (so the user can still backspace)
	if(strlen(Buffer) >= maxChar)
		return;
	
	int bShift = ( (App.pdistruct->keys[DIK_RSHIFT] & 0x80) ||
				   (App.pdistruct->keys[DIK_LSHIFT] & 0x80) );


	//***Special Keys
	/*
	if(pKeys[DIK_TAB] & 0x80)
		strcat(Buffer, "    ");
	if(pKeys[DIK_SPACE] & 0x80)
		strcat(Buffer, " ");
		*/

	if(GameData.keysDown[VK_SPACE])
	{
		GameData.keysDown[VK_SPACE] = 0;
		strcat(Buffer, " ");
	}

	/*
	if(pKeys[DIK_TAB] == 1)
		strcat(Buffer, "    ");
	if(pKeys[DIK_SPACE] == 1)
		strcat(Buffer, " ");
		*/



	//if(pKeys[DIK_NUMLOCK] & 0x80)
	//	strcat(Buffer, "numlock");


	//***Shift Keys
	if(bShift)
	{
		//***1st row of shift keys
		/*
		if(pKeys[DIK_0] & 0x80)
			strcat(Buffer, ")");
		else if(pKeys[DIK_1] & 0x80)
			strcat(Buffer, "!");
		else if(pKeys[DIK_2] & 0x80)
			strcat(Buffer, "@");
		else if(pKeys[DIK_3] & 0x80)
			strcat(Buffer, "#");
		else if(pKeys[DIK_4] & 0x80)
			strcat(Buffer, "$");
		else if(pKeys[DIK_5] & 0x80)
			strcat(Buffer, "%");
		else if(pKeys[DIK_6] & 0x80)
			strcat(Buffer, "^");
		else if(pKeys[DIK_7] & 0x80)
			strcat(Buffer, "&");
		else if(pKeys[DIK_8] & 0x80)
			strcat(Buffer, "*");
		else if(pKeys[DIK_9] & 0x80)
			strcat(Buffer, "(");
		else if(pKeys[DIK_MINUS] & 0x80)
			strcat(Buffer, "_");
		else if(pKeys[DIK_EQUALS] & 0x80)
			strcat(Buffer, "+");
		else if(pKeys[DIK_BACKSLASH] & 0x80)
			strcat(Buffer, "|");
			*/
		
		/*
		if(pKeys[DIK_0] == 1)
			strcat(Buffer, ")");
		else if(pKeys[DIK_1] == 1)
			strcat(Buffer, "!");
		else if(pKeys[DIK_2] == 1)
			strcat(Buffer, "@");
		else if(pKeys[DIK_3] == 1)
			strcat(Buffer, "#");
		else if(pKeys[DIK_4] == 1)
			strcat(Buffer, "$");
		else if(pKeys[DIK_5] == 1)
			strcat(Buffer, "%");
		else if(pKeys[DIK_6] == 1)
			strcat(Buffer, "^");
		else if(pKeys[DIK_7] == 1)
			strcat(Buffer, "&");
		else if(pKeys[DIK_8] == 1)
			strcat(Buffer, "*");
		else if(pKeys[DIK_9] == 1)
			strcat(Buffer, "(");
		else if(pKeys[DIK_MINUS] == 1)
			strcat(Buffer, "_");
		else if(pKeys[DIK_EQUALS] == 1)
			strcat(Buffer, "+");
		else if(pKeys[DIK_BACKSLASH] == 1)
			strcat(Buffer, "|");
			*/

		//*** 2nd row of shift keys
		/*
		else if(pKeys[DIK_LBRACKET] & 0x80)
			strcat(Buffer, "{");
		else if(pKeys[DIK_RBRACKET] & 0x80)
			strcat(Buffer, "}");
			*/

		/*
		else if(pKeys[DIK_LBRACKET] == 1)
			strcat(Buffer, "{");
		else if(pKeys[DIK_RBRACKET] == 1)
			strcat(Buffer, "}");
			*/
		
		//*** 3rd row of shift keys
		/*
		else if(pKeys[DIK_SEMICOLON] & 0x80)
			strcat(Buffer, ":");
		else if(pKeys[DIK_APOSTROPHE] & 0x80)
			strcat(Buffer, "\"");
			*/

		/*
		else if(pKeys[DIK_SEMICOLON] == 1)
			strcat(Buffer, ":");
		else if(pKeys[DIK_APOSTROPHE] == 1)
			strcat(Buffer, "\"");
			*/

		//*** 4th row of shift keys
		/*
		else if(pKeys[DIK_COMMA] & 0x80)
			strcat(Buffer, "<");
		else if(pKeys[DIK_PERIOD] & 0x80)
			strcat(Buffer, ">");
		else if(pKeys[DIK_SLASH] & 0x80)
			strcat(Buffer, "?");
			*/

		/*
		else if(pKeys[DIK_COMMA] == 1)
			strcat(Buffer, "<");
		else if(pKeys[DIK_PERIOD] == 1)
			strcat(Buffer, ">");
		else if(pKeys[DIK_SLASH] == 1)
			strcat(Buffer, "?");
			*/

	}//end Shift keys
	else
	{
		//***1 row  non shift keys
		/*
		if(pKeys[DIK_GRAVE] & 0x80)
			strcat(Buffer, "`");
		else if(pKeys[DIK_1] & 0x80)
			strcat(Buffer, "1");
		else if(pKeys[DIK_2] & 0x80)
			strcat(Buffer, "2");
		else if(pKeys[DIK_3] & 0x80)
			strcat(Buffer, "3");
		else if(pKeys[DIK_4] & 0x80)
			strcat(Buffer, "4");
		else if(pKeys[DIK_5] & 0x80)
			strcat(Buffer, "5");
		else if(pKeys[DIK_6] & 0x80)
			strcat(Buffer, "6");
		else if(pKeys[DIK_7] & 0x80)
			strcat(Buffer, "7");
		else if(pKeys[DIK_8] & 0x80)
			strcat(Buffer, "8");
		else if(pKeys[DIK_9] & 0x80)
			strcat(Buffer, "9");
		else if(pKeys[DIK_0] & 0x80)
			strcat(Buffer, "0");
		else if(pKeys[DIK_MINUS] & 0x80)
			strcat(Buffer, "-");
		else if(pKeys[DIK_EQUALS] & 0x80)
			strcat(Buffer, "=");
		else if(pKeys[DIK_BACKSLASH] & 0x80)
			strcat(Buffer, "\\");
			*/
		if(GameData.keysDown['0'])
		{
			GameData.keysDown['0'] = 0;
			strcat(Buffer, "0");
		}
		else if(GameData.keysDown['1'])
		{
			GameData.keysDown['1'] = 0;
			strcat(Buffer, "1");
		}
		else if(GameData.keysDown['2'])
		{
			GameData.keysDown['2'] = 0;
			strcat(Buffer, "2");
		}
		else if(GameData.keysDown['3'])
		{
			GameData.keysDown['3'] = 0;
			strcat(Buffer, "3");
		}
		else if(GameData.keysDown['4'])
		{
			GameData.keysDown['4'] = 0;
			strcat(Buffer, "4");
		}
		else if(GameData.keysDown['5'])
		{
			GameData.keysDown['5'] = 0;
			strcat(Buffer, "5");
		}
		else if(GameData.keysDown['6'])
		{
			GameData.keysDown['6'] = 0;
			strcat(Buffer, "6");
		}
		else if(GameData.keysDown['7'])
		{
			GameData.keysDown['7'] = 0;
			strcat(Buffer, "7");
		}
		else if(GameData.keysDown['8'])
		{
			GameData.keysDown['8'] = 0;
			strcat(Buffer, "8");
		}
		else if(GameData.keysDown['9'])
		{
			GameData.keysDown['9'] = 0;
			strcat(Buffer, "9");
		}

		/*
		if(pKeys[DIK_GRAVE] == 1)
			strcat(Buffer, "`");
		else if(pKeys[DIK_1] == 1)
			strcat(Buffer, "1");
		else if(pKeys[DIK_2] == 1)
			strcat(Buffer, "2");
		else if(pKeys[DIK_3] == 1)
			strcat(Buffer, "3");
		else if(pKeys[DIK_4] == 1)
			strcat(Buffer, "4");
		else if(pKeys[DIK_5] == 1)
			strcat(Buffer, "5");
		else if(pKeys[DIK_6] == 1)
			strcat(Buffer, "6");
		else if(pKeys[DIK_7] == 1)
			strcat(Buffer, "7");
		else if(pKeys[DIK_8] == 1)
			strcat(Buffer, "8");
		else if(pKeys[DIK_9] == 1)
			strcat(Buffer, "9");
		else if(pKeys[DIK_0] == 1)
			strcat(Buffer, "0");
		else if(pKeys[DIK_MINUS] == 1)
			strcat(Buffer, "-");
		else if(pKeys[DIK_EQUALS] == 1)
			strcat(Buffer, "=");
		else if(pKeys[DIK_BACKSLASH] == 1)
			strcat(Buffer, "\\");
			*/
		
		//***2nd row of non shift keys
		/*
		else if(pKeys[DIK_LBRACKET] & 0x80)
			strcat(Buffer, "[");
		else if(pKeys[DIK_RBRACKET] & 0x80)
			strcat(Buffer, "]");
			*/

		/*
		else if(pKeys[DIK_LBRACKET] == 1)
			strcat(Buffer, "[");
		else if(pKeys[DIK_RBRACKET] == 1)
			strcat(Buffer, "]");
			*/

		//***3rd row of non shift keys
		/*
		else if(pKeys[DIK_SEMICOLON] & 0x80)
			strcat(Buffer, ";");
		else if(pKeys[DIK_APOSTROPHE] & 0x80)
			strcat(Buffer, "'");
			*/

		/*
		else if(pKeys[DIK_SEMICOLON] == 1)
			strcat(Buffer, ";");
		else if(pKeys[DIK_APOSTROPHE] == 1)
			strcat(Buffer, "'");
			*/

		//***4th row of non shift keys
		/*
		else if(pKeys[DIK_COMMA] & 0x80)
			strcat(Buffer, ",");
		else if(pKeys[DIK_PERIOD] & 0x80)
			strcat(Buffer, ".");
		else if(pKeys[DIK_SLASH] & 0x80)
			strcat(Buffer, "/");
			*/

		/*
		else if(pKeys[DIK_COMMA] == 1)
			strcat(Buffer, ",");
		else if(pKeys[DIK_PERIOD] == 1)
			strcat(Buffer, ".");
		else if(pKeys[DIK_SLASH] == 1)
			strcat(Buffer, "/");
			*/
	}//end else


	//***2nd row of keys
	/*
	if(pKeys[DIK_Q] & 0x80)
		strcat(Buffer, "Q");
	else if(pKeys[DIK_W] & 0x80)
		strcat(Buffer, "W");
	else if(pKeys[DIK_E] & 0x80)
		strcat(Buffer, "E");
	else if(pKeys[DIK_R] & 0x80)
		strcat(Buffer, "R");
	else if(pKeys[DIK_T] & 0x80)
		strcat(Buffer, "T");
	else if(pKeys[DIK_Y] & 0x80)
		strcat(Buffer, "Y");
	else if(pKeys[DIK_U] & 0x80)
		strcat(Buffer, "U");
	else if(pKeys[DIK_I] & 0x80)
		strcat(Buffer, "I");
	else if(pKeys[DIK_O] & 0x80)
		strcat(Buffer, "O");
	else if(pKeys[DIK_P] & 0x80)
		strcat(Buffer, "P");
	*/


	if(GameData.keysDown['Q'])
	{
		GameData.keysDown['Q'] = 0;
		strcat(Buffer, "Q");
	}
	else if(GameData.keysDown['W'])
	{
		GameData.keysDown['W'] = 0;
		strcat(Buffer, "W");
	}
	else if(GameData.keysDown['E'])
	{
		GameData.keysDown['E'] = 0;
		strcat(Buffer, "E");
	}
	else if(GameData.keysDown['R'])
	{
		GameData.keysDown['R'] = 0;
		strcat(Buffer, "R");
	}
	else if(GameData.keysDown['T'])
	{
		GameData.keysDown['T'] = 0;
		strcat(Buffer, "T");
	}
	else if(GameData.keysDown['Y'])
	{
		GameData.keysDown['Y'] = 0;
		strcat(Buffer, "Y");
	}
	else if(GameData.keysDown['U'])
	{
		GameData.keysDown['U'] = 0;
		strcat(Buffer, "U");
	}
	else if(GameData.keysDown['I'])
	{
		GameData.keysDown['I'] = 0;
		strcat(Buffer, "I");
	}
	else if(GameData.keysDown['O'])
	{
		GameData.keysDown['O'] = 0;
		strcat(Buffer, "O");
	}
	else if(GameData.keysDown['P'])
	{
		GameData.keysDown['P'] = 0;
		strcat(Buffer, "P");
	}

	/*
	if(pKeys[DIK_Q] == 1)
		strcat(Buffer, "Q");
	else if(pKeys[DIK_W] == 1)
		strcat(Buffer, "W");
	else if(pKeys[DIK_E] == 1)
		strcat(Buffer, "E");
	else if(pKeys[DIK_R] == 1)
		strcat(Buffer, "R");
	else if(pKeys[DIK_T] == 1)
		strcat(Buffer, "T");
	else if(pKeys[DIK_Y] == 1)
		strcat(Buffer, "Y");
	else if(pKeys[DIK_U] == 1)
		strcat(Buffer, "U");
	else if(pKeys[DIK_I] == 1)
		strcat(Buffer, "I");
	else if(pKeys[DIK_O] == 1)
		strcat(Buffer, "O");
	else if(pKeys[DIK_P] == 1)
		strcat(Buffer, "P");
		*/
	
	//***3rd row of keys
	/*
	else if(pKeys[DIK_A] & 0x80)
		strcat(Buffer, "A");
	else if(pKeys[DIK_S] & 0x80)
		strcat(Buffer, "S");
	else if(pKeys[DIK_D] & 0x80)
		strcat(Buffer, "D");
	else if(pKeys[DIK_F] & 0x80)
		strcat(Buffer, "F");
	else if(pKeys[DIK_G] & 0x80)
		strcat(Buffer, "G");
	else if(pKeys[DIK_H] & 0x80)
		strcat(Buffer, "H");
	else if(pKeys[DIK_J] & 0x80)
		strcat(Buffer, "J");
	else if(pKeys[DIK_K] & 0x80)
		strcat(Buffer, "K");
	else if(pKeys[DIK_L] & 0x80)
		strcat(Buffer, "L");
		*/

	if(GameData.keysDown['A'])
	{
		GameData.keysDown['A'] = 0;
		strcat(Buffer, "A");
	}
	else if(GameData.keysDown['S'])
	{
		GameData.keysDown['S'] = 0;
		strcat(Buffer, "S");
	}
	else if(GameData.keysDown['D'])
	{
		GameData.keysDown['D'] = 0;
		strcat(Buffer, "D");
	}
	else if(GameData.keysDown['F'])
	{
		GameData.keysDown['F'] = 0;
		strcat(Buffer, "F");
	}
	else if(GameData.keysDown['G'])
	{
		GameData.keysDown['G'] = 0;
		strcat(Buffer, "G");
	}
	else if(GameData.keysDown['H'])
	{
		GameData.keysDown['H'] = 0;
		strcat(Buffer, "H");
	}
	else if(GameData.keysDown['J'])
	{
		GameData.keysDown['J'] = 0;
		strcat(Buffer, "J");
	}
	else if(GameData.keysDown['K'])
	{
		GameData.keysDown['K'] = 0;
		strcat(Buffer, "K");
	}
	else if(GameData.keysDown['L'])
	{
		GameData.keysDown['L'] = 0;
		strcat(Buffer, "L");
	}

	/*
	else if(pKeys[DIK_A] == 1)
		strcat(Buffer, "A");
	else if(pKeys[DIK_S] == 1)
		strcat(Buffer, "S");
	else if(pKeys[DIK_D] == 1)
		strcat(Buffer, "D");
	else if(pKeys[DIK_F] == 1)
		strcat(Buffer, "F");
	else if(pKeys[DIK_G] == 1)
		strcat(Buffer, "G");
	else if(pKeys[DIK_H] == 1)
		strcat(Buffer, "H");
	else if(pKeys[DIK_J] == 1)
		strcat(Buffer, "J");
	else if(pKeys[DIK_K] == 1)
		strcat(Buffer, "K");
	else if(pKeys[DIK_L] == 1)
		strcat(Buffer, "L");
		*/

	//***4th row of keys
	/*
	else if(pKeys[DIK_Z] & 0x80)
		strcat(Buffer, "Z");
	else if(pKeys[DIK_X] & 0x80)
		strcat(Buffer, "X");
	else if(pKeys[DIK_C] & 0x80)
		strcat(Buffer, "C");
	else if(pKeys[DIK_V] & 0x80)
		strcat(Buffer, "V");
	else if(pKeys[DIK_B] & 0x80)
		strcat(Buffer, "B");
	else if(pKeys[DIK_N] & 0x80)
		strcat(Buffer, "N");
	else if(pKeys[DIK_M] & 0x80)
		strcat(Buffer, "M");
		*/

	if(GameData.keysDown['Z'])
	{
		GameData.keysDown['Z'] = 0;
		strcat(Buffer, "Z");
	}
	else if(GameData.keysDown['X'])
	{
		GameData.keysDown['X'] = 0;
		strcat(Buffer, "X");
	}
	else if(GameData.keysDown['C'])
	{
		GameData.keysDown['C'] = 0;
		strcat(Buffer, "C");
	}
	else if(GameData.keysDown['V'])
	{
		GameData.keysDown['V'] = 0;
		strcat(Buffer, "V");
	}
	else if(GameData.keysDown['B'])
	{
		GameData.keysDown['B'] = 0;
		strcat(Buffer, "B");
	}
	else if(GameData.keysDown['N'])
	{
		GameData.keysDown['N'] = 0;
		strcat(Buffer, "N");
	}
	else if(GameData.keysDown['M'])
	{
		GameData.keysDown['M'] = 0;
		strcat(Buffer, "M");
	}

	/*
	else if(pKeys[DIK_Z] == 1)
		strcat(Buffer, "Z");
	else if(pKeys[DIK_X] == 1)
		strcat(Buffer, "X");
	else if(pKeys[DIK_C] == 1)
		strcat(Buffer, "C");
	else if(pKeys[DIK_V] == 1)
		strcat(Buffer, "V");
	else if(pKeys[DIK_B] == 1)
		strcat(Buffer, "B");
	else if(pKeys[DIK_N] == 1)
		strcat(Buffer, "N");
	else if(pKeys[DIK_M] == 1)
		strcat(Buffer, "M");
		*/

	//***NumBad Keys
	/*
	else if(pKeys[DIK_NUMPAD0] & 0x80)
		strcat(Buffer, "0");
	else if(pKeys[DIK_NUMPAD1] & 0x80)
		strcat(Buffer, "1");
	else if(pKeys[DIK_NUMPAD2] & 0x80)
		strcat(Buffer, "2");
	else if(pKeys[DIK_NUMPAD3] & 0x80)
		strcat(Buffer, "3");
	else if(pKeys[DIK_NUMPAD4] & 0x80)
		strcat(Buffer, "4");
	else if(pKeys[DIK_NUMPAD5] & 0x80)
		strcat(Buffer, "5");
	else if(pKeys[DIK_NUMPAD6] & 0x80)
		strcat(Buffer, "6");
	else if(pKeys[DIK_NUMPAD7] & 0x80)
		strcat(Buffer, "7");
	else if(pKeys[DIK_NUMPAD8] & 0x80)
		strcat(Buffer, "8");
	else if(pKeys[DIK_NUMPAD9] & 0x80)
		strcat(Buffer, "9");
	else if(pKeys[DIK_SUBTRACT] & 0x80)
		strcat(Buffer, "-");
	else if(pKeys[DIK_ADD] & 0x80)
		strcat(Buffer, "+");
		*/

	if(GameData.keysDown[VK_NUMPAD0])
	{
		GameData.keysDown['0'] = 0;
		strcat(Buffer, "0");
	}
	else if(GameData.keysDown[VK_NUMPAD1])
	{
		GameData.keysDown['1'] = 0;
		strcat(Buffer, "1");
	}
	else if(GameData.keysDown[VK_NUMPAD2])
	{
		GameData.keysDown['2'] = 0;
		strcat(Buffer, "2");
	}
	else if(GameData.keysDown[VK_NUMPAD3])
	{
		GameData.keysDown['3'] = 0;
		strcat(Buffer, "3");
	}
	else if(GameData.keysDown[VK_NUMPAD4])
	{
		GameData.keysDown['4'] = 0;
		strcat(Buffer, "4");
	}
	else if(GameData.keysDown[VK_NUMPAD5])
	{
		GameData.keysDown['5'] = 0;
		strcat(Buffer, "5");
	}
	else if(GameData.keysDown[VK_NUMPAD6])
	{
		GameData.keysDown['6'] = 0;
		strcat(Buffer, "6");
	}
	else if(GameData.keysDown[VK_NUMPAD7])
	{
		GameData.keysDown['7'] = 0;
		strcat(Buffer, "7");
	}
	else if(GameData.keysDown[VK_NUMPAD8])
	{
		GameData.keysDown['8'] = 0;
		strcat(Buffer, "8");
	}
	else if(GameData.keysDown[VK_NUMPAD9])
	{
		GameData.keysDown['9'] = 0;
		strcat(Buffer, "9");
	}

/*
	else if(pKeys[DIK_NUMPAD0] == 1)
		strcat(Buffer, "0");
	else if(pKeys[DIK_NUMPAD1] == 1)
		strcat(Buffer, "1");
	else if(pKeys[DIK_NUMPAD2] == 1)
		strcat(Buffer, "2");
	else if(pKeys[DIK_NUMPAD3] == 1)
		strcat(Buffer, "3");
	else if(pKeys[DIK_NUMPAD4] == 1)
		strcat(Buffer, "4");
	else if(pKeys[DIK_NUMPAD5] == 1)
		strcat(Buffer, "5");
	else if(pKeys[DIK_NUMPAD6] == 1)
		strcat(Buffer, "6");
	else if(pKeys[DIK_NUMPAD7] == 1)
		strcat(Buffer, "7");
	else if(pKeys[DIK_NUMPAD8] == 1)
		strcat(Buffer, "8");
	else if(pKeys[DIK_NUMPAD9] == 1)
		strcat(Buffer, "9");
	else if(pKeys[DIK_SUBTRACT] == 1)
		strcat(Buffer, "-");
	else if(pKeys[DIK_ADD] == 1)
		strcat(Buffer, "+");
		*/
/*
	//******Keys that I'm not using
	if(pKeys[DIK_MULTIPLY] & 0x80)
		strcat(Buffer, "*");	
	
	if(pKeys[DIK_DECIMAL] & 0x80)
		strcat(Buffer, ".");

	if(pKeys[DIK_DIVIDE] & 0x80)
		strcat(Buffer, "/");

	if(pKeys[DIK_UP] & 0x80)
		strcat(Buffer, "up arrow");
	if(pKeys[DIK_PRIOR] & 0x80)
		strcat(Buffer, "page up");
	if(pKeys[DIK_LEFT] & 0x80)
		strcat(Buffer, "left arrow");
	if(pKeys[DIK_RIGHT] & 0x80)
		strcat(Buffer, "right arrow");
	if(pKeys[DIK_END] & 0x80)
		strcat(Buffer, "end");
	if(pKeys[DIK_DOWN] & 0x80)
		strcat(Buffer, "down");
	if(pKeys[DIK_NEXT] & 0x80)
		strcat(Buffer, "page down");
	if(pKeys[DIK_INSERT] & 0x80)
		strcat(Buffer, "insert");
	if(pKeys[DIK_DELETE] & 0x80)
		strcat(Buffer, "delete");
	if(pKeys[DIK_LWIN] & 0x80)
		strcat(Buffer, "left win");
	if(pKeys[DIK_RWIN] & 0x80)
		strcat(Buffer, "right win");
	if(pKeys[DIK_DOWN] & 0x80)
		strcat(Buffer, "down");
	if(pKeys[DIK_NEXT] & 0x80)
		strcat(Buffer, "page down");
	if(pKeys[DIK_HOME] & 0x80)
		strcat(Buffer, "home");
*/

	//***Test the size of our buffer again... if it has changed then a key
	//   has been pressed.. therefore change our keypressed flag.
//	if(strlen(Buffer) != StrLength);
//		RepeatTimer = REPEAT_WAIT_TIME;
	


}