#include <ddraw.h>
#include <stdio.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>

#include "./mmgr.h"

//#include "database.h"
#include "DPlay.h"
#include "app.h"
#include "gamemenu.h"
#include "dinput.h"

#include "text.h"


extern APP			App;
extern GAME_DATA	GameData;

#define	REPEAT_WAIT_TIME	10

int IPInput(char *Buffer, unsigned int CurrIPChunk);

int DisplayIP(Game_Menu *pGameMenu, Menu_Button *pButton)
{
	int		i;
	POINT	screenPos		= pButton->GetScreenPos();
	
	char	*pCurrIPAddress;
	
	//***Check if the user wants to finnaly enter the IP
//	if(((App.pdistruct->keys[DIK_RETURN] & 0x80 ) || 
//	    (App.pdistruct->keys[DIK_NUMPADENTER] & 0x80) ) && 
//		GameData.ChrIndex == 3  && strlen(GameData.IPAddress[3]) )
	if((GameData.keysDown[VK_RETURN] ) ||  
		GameData.ChrIndex == 3  && strlen(GameData.IPAddress[3]) )
	{
		GameData.keysDown[VK_RETURN] = 0;
		//***Test to see if it's a correct IP address
		int IP[4]	= { atoi(GameData.IPAddress[0]),
						atoi(GameData.IPAddress[1]),
						atoi(GameData.IPAddress[2]),
						atoi(GameData.IPAddress[3])};
		int bError=0;

		//***IP input error checking
		if( !(IP[0] +  IP[1] + IP[2] + IP[3]) )
			bError = 1;

		for( i=0; i<3; i++)
		{
			if(IP[i] > 255 || IP[i] < 0) //(simple check)
				bError = 1;
		}

		//last number can't be 255
		for( i=3; i<4; i++)
		{
			if(IP[i] > 254 || IP[i] < 0) //(simple check)
				bError = 1;
		}

		if(bError)
		{
			GameData.ChrIndex			= 0;
			GameData.IPAddress[0][0]	= NULL;
			GameData.IPAddress[1][0]	= NULL;
			GameData.IPAddress[2][0]	= NULL;
			GameData.IPAddress[3][0]	= NULL;
			
			pGameMenu->buttonArray[43].Enable(0);//enable our IP error box
		}
		else //CONVERT VALID IP ADDRESS AND SEND IT ALONG ITS WAY
			return 1;
		
	}
	

	//***Set the Current Text pointer (because there are 4 of em')
	if(GameData.ChrIndex == 0)
		pCurrIPAddress = GameData.IPAddress[0];
	else if(GameData.ChrIndex == 1)
		pCurrIPAddress = GameData.IPAddress[1];
	else if(GameData.ChrIndex == 2)
		pCurrIPAddress = GameData.IPAddress[2];
	else
	{
		pCurrIPAddress			= GameData.IPAddress[3];
		GameData.ChrIndex	= 3;//This makes sure all ChrIndex above 2 is == 3
	}


	//***Get our Input from the keyboard
	GameData.ChrIndex += IPInput(pCurrIPAddress, GameData.ChrIndex);

	//***Draw our IP 4 numbers (neat style)
	U32	OffSet0 = 12 - (4 * strlen(GameData.IPAddress[0]) );
	U32	OffSet1 = 12 - (4 * strlen(GameData.IPAddress[1]) );
	U32	OffSet2 = 12 - (4 * strlen(GameData.IPAddress[2]) );
	U32	OffSet3 = 12 - (4 * strlen(GameData.IPAddress[3]) );
	
	if(!App.UserSettings.RenderMode)
	{
		App.pDDraw->DDrawText(screenPos.x +  85 + OffSet0, screenPos.y + 9, TF_FONT_SMALL | TF_LENGTH_IP_INPUT | TF_SPACING_0, GameData.IPAddress[0]);
		App.pDDraw->DDrawText(screenPos.x + 115 + OffSet1, screenPos.y + 9, TF_FONT_SMALL | TF_LENGTH_IP_INPUT | TF_SPACING_0, GameData.IPAddress[1]);
		App.pDDraw->DDrawText(screenPos.x + 145 + OffSet2, screenPos.y + 9, TF_FONT_SMALL | TF_LENGTH_IP_INPUT | TF_SPACING_0, GameData.IPAddress[2]);
		App.pDDraw->DDrawText(screenPos.x + 175 + OffSet3, screenPos.y + 9, TF_FONT_SMALL | TF_LENGTH_IP_INPUT | TF_SPACING_0, GameData.IPAddress[3]);
	}
	else
	{
		App.pOpenGL->GLDrawText(screenPos.x +  85 + OffSet0, screenPos.y + 9, TF_FONT_SMALL | TF_LENGTH_IP_INPUT | TF_SPACING_0, GameData.IPAddress[0]);
		App.pOpenGL->GLDrawText(screenPos.x + 115 + OffSet1, screenPos.y + 9, TF_FONT_SMALL | TF_LENGTH_IP_INPUT | TF_SPACING_0, GameData.IPAddress[1]);
		App.pOpenGL->GLDrawText(screenPos.x + 145 + OffSet2, screenPos.y + 9, TF_FONT_SMALL | TF_LENGTH_IP_INPUT | TF_SPACING_0, GameData.IPAddress[2]);
		App.pOpenGL->GLDrawText(screenPos.x + 175 + OffSet3, screenPos.y + 9, TF_FONT_SMALL | TF_LENGTH_IP_INPUT | TF_SPACING_0, GameData.IPAddress[3]);
	}

	
	return 0;
}

int IPInput(char *Buffer, unsigned int CurrIPChunk)
{
	unsigned int	StrLength	= strlen(Buffer);
	
	U32*			pKeys		= App.pdistruct->curKeyState;

	//***Check ENTER and RETURN
//	if((App.pdistruct->keys[DIK_RETURN] & 0x80) ||
//	   (App.pdistruct->keys[DIK_NUMPADENTER] & 0x80)  )
	if(GameData.keysDown[VK_RETURN])
	{
		GameData.keysDown[VK_RETURN] = 0;
		if( (!strcmp(Buffer, "0")) || (!strcmp(Buffer, "00"))  )
		{
			Buffer[0]	= NULL;
			strcat(Buffer, "0");
			return 1;
		}
		else if(StrLength > 0)	//don't go to the nextIPChunk unless there is something first
		{	
			pKeys[DIK_RETURN] = 0;
			pKeys[DIK_NUMPADENTER] = 0;
			return 1;
		}
	}
	//***Check for BackSpace
	if(GameData.keysDown[VK_BACK])
//	else if(pKeys[DIK_BACK] == 1)
	{
		if(StrLength)
		{
			Buffer[StrLength - 1] = NULL;
			return 0;// stay in the same IP chunk (we havn't deleted all 3 chars yet)
		}
		else if(!StrLength && CurrIPChunk != 0)
		{
			// go back an IP address
			GameData.ChrIndex--;

			if(GameData.ChrIndex == 0)
				Buffer = GameData.IPAddress[0];
			else if(GameData.ChrIndex == 1)
				Buffer = GameData.IPAddress[1];
			else if(GameData.ChrIndex == 2)
				Buffer = GameData.IPAddress[2];

			Buffer[2] = NULL;
			return 0;
		}
		GameData.keysDown[VK_BACK] = 0;	
	}

	//Make sure that we don't add any more characters past the last character of the last chunkr
	if(strlen(Buffer) == 3 && CurrIPChunk == 3)
		return 0;

	//***Number Keys

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
	if(pKeys[DIK_1]  == 1)
		strcat(Buffer, "1");
	else if(pKeys[DIK_2]  == 1)
		strcat(Buffer, "2");
	else if(pKeys[DIK_3]  == 1)
		strcat(Buffer, "3");
	else if(pKeys[DIK_4]  == 1)
		strcat(Buffer, "4");
	else if(pKeys[DIK_5]  == 1)
		strcat(Buffer, "5");
	else if(pKeys[DIK_6]  == 1)
		strcat(Buffer, "6");
	else if(pKeys[DIK_7]  == 1)
		strcat(Buffer, "7");
	else if(pKeys[DIK_8]  == 1)
		strcat(Buffer, "8");
	else if(pKeys[DIK_9]  == 1)
		strcat(Buffer, "9");
	else if(pKeys[DIK_0]  == 1)
		strcat(Buffer, "0");
		*/

	//***NumBad Keys

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
		*/

	StrLength	= strlen(Buffer);

	
	if(StrLength == 3)
	{
		//***CUT THE ZERO SUCK***
		
		if((!strcmp(Buffer, "0"  )) || 
		   (!strcmp(Buffer, "00" )) || 
		   (!strcmp(Buffer, "000"))  )
		{
			Buffer[0]	= NULL;
			strcat(Buffer, "0");
		}
		else if(Buffer[0] == '0')
		{
			for(int i=0; i<3;i++)
				Buffer[i] = Buffer[i+1];
			
			if(Buffer[0] == '0')
				for(i=0; i<2;i++)
					Buffer[i] = Buffer[i+1];
		}
		return 1;
	}
	else
		return 0;

}
