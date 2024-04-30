#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "dinput.h"
#include "defines.h"


#include "vector.h"
#include "model.h"
#include "gem3d.h"
#include "camera3d.h"
#include "player.h"
#include "board.h"


extern APP App;
extern GAME_DATA GameData;




Camera3D::Camera3D()
{
	cameraFlags		= 0;
	moveDirection	= 0;

	worldPos.x		= 0;
	worldPos.y		= 0;
	worldPos.z		= 0;

	targetPos		= worldPos;

	rotation.x		= 0;
	rotation.y		= 0; 
	rotation.z		= 0;

	headlight.x		= 0;
	headlight.y		= 0;
	headlight.z		= 0;

	targetPos.x		= 0;
	targetPos.y		= 0;
	targetPos.z		= 0;

	velocity.x		= 0;
	velocity.y		= 0;
	velocity.z		= 0;
}




void Camera3D::Update()
{
	if(cameraFlags & CAMERA_FLAG_CHECK_MOVE)
	{
		HandleMovement();
	
		cameraFlags &= ~CAMERA_FLAG_CHECK_MOVE;
	}

	UpdateVeloc();
	UpdatePos();

	GetDirectionVector();
}




void Camera3D::HandleMovement()
{
	if(!App.UserSettings.RenderMode)
	{
		if(cameraFlags & CAMERA_FLAG_MOVE_UP)
			targetPos.y += 26;

		else if(cameraFlags & CAMERA_FLAG_MOVE_DOWN)
			targetPos.y	-= 26;

		else if(cameraFlags & CAMERA_FLAG_MOVE_LEFT)
			targetPos.x	-= 30;

		else if(cameraFlags & CAMERA_FLAG_MOVE_RIGHT)
			targetPos.x	+= 30;
	}
	else
	{
		if(cameraFlags & CAMERA_FLAG_MOVE_UP)
			targetPos.y -= 26;

		else if(cameraFlags & CAMERA_FLAG_MOVE_DOWN)
			targetPos.y	+= 26;

		else if(cameraFlags & CAMERA_FLAG_MOVE_LEFT)
			targetPos.x	+= 30;

		else if(cameraFlags & CAMERA_FLAG_MOVE_RIGHT)
			targetPos.x	-= 30;
	}

	if(cameraFlags & CAMERA_FLAG_ZOOM_IN)
	{
		if((targetPos.z += 128) > 512)
			targetPos.z	= 512;
	}

	else if(cameraFlags & CAMERA_FLAG_ZOOM_OUT)
		targetPos.z	-= 128;
}





/*
void Menu_Button::UpdatePos()
{
	// update position
	screenPos.x	= (int)(screenX += velocX);
	screenPos.y	= (int)(screenY += velocY);

	if(IsActiveEnabled())
	{
		if(	(screenPos.x >= enabledPos.x - 1) &&
			(screenPos.x <= enabledPos.x + 1) )
			screenPos.x	= enabledPos.x;
	}

	if(IsActiveDisabled())
	{
		if(	(screenPos.x >= disabledPos.x - 1) &&
			(screenPos.x <= disabledPos.x + 1) )
			screenPos.x	= disabledPos.x;
	}
}




void Menu_Button::UpdateVeloc()
{
	// update velocity
	if(IsActiveEnabled())
	{
		velocX		= (enabledPos.x - screenX) / 8;
		velocY		= (enabledPos.y - screenY) / 8;
	}
	else if(IsActiveDisabled())
	{
		velocX		= (disabledPos.x - screenX) / 8;
		velocY		= (disabledPos.y - screenY) / 8;
	}
}
*/

void Camera3D::UpdateVeloc()
{
	velocity.x	= (targetPos.x - worldPos.x) * 0.1f;
	velocity.y	= (targetPos.y - worldPos.y) * 0.1f;
	velocity.z	= (targetPos.z - worldPos.z) * 0.1f;

	(velocity.x > 0) ?	velocity.x = LIMIT(velocity.x, 1, 1000) : velocity.x = LIMIT(velocity.x, -1000, -1);
	(velocity.y > 0) ?	velocity.y = LIMIT(velocity.y, 1, 1000) : velocity.y = LIMIT(velocity.y, -1000, -1);
	(velocity.z > 0) ?	velocity.z = LIMIT(velocity.z, 1, 1000) : velocity.z = LIMIT(velocity.z, -1000, -1);
}

void Camera3D::UpdatePos()
{
	if( ((worldPos.x += velocity.x) >= (targetPos.x - 1)) &&
		(worldPos.x <= (targetPos.x + 1)) )
		worldPos.x	= targetPos.x;
	
	if( ((worldPos.y += velocity.y) >= (targetPos.y - 1)) &&
		(worldPos.y <= (targetPos.y + 1)) )
		worldPos.y	= targetPos.y;

	if( ((worldPos.z += velocity.z) >= (targetPos.z - 1)) &&
		(worldPos.z <= (targetPos.z + 1)) )
		worldPos.z	= targetPos.z;
}




void Camera3D::GetDirectionVector()
{
	// default headlight position is (0, 0, 1)
	float		c1		= App.Cos[(int)rotation.x],
				c2		= App.Cos[(int)rotation.y],
				s1		= App.Sin[(int)rotation.x],
				s2		= App.Sin[(int)rotation.y];

	headlight.x			= s2;
	headlight.y			= s1 * c2;
	headlight.z			= c1 * c2;
	headlight			= ~headlight;

}


void Camera3D::HandleKeyInput()
{

	char	*keyFlags	= GameData.keysDown;

#ifdef DINPUT
	// check for camera movement along the x-axis
//	if(App.pdistruct->keys[DIK_RIGHT] & 0x80)
//		worldPos.x += 4;
//	if(App.pdistruct->keys[DIK_LEFT] & 0x80)
//		worldPos.x -= 4;
	if(App.pdistruct->keys[DIK_H] & 0x80)
		cameraFlags		= CAMERA_FLAG_MOVE_LEFT | CAMERA_FLAG_CHECK_MOVE;
	if(App.pdistruct->keys[DIK_K] & 0x80)
		cameraFlags		= CAMERA_FLAG_MOVE_RIGHT | CAMERA_FLAG_CHECK_MOVE;


	// check for camera movement along the y-axis
	if(App.pdistruct->keys[DIK_U] & 0x80)
		cameraFlags		= CAMERA_FLAG_MOVE_UP | CAMERA_FLAG_CHECK_MOVE;
	if(App.pdistruct->keys[DIK_J] & 0x80)
		cameraFlags		= CAMERA_FLAG_MOVE_DOWN | CAMERA_FLAG_CHECK_MOVE;
//	if(App.pdistruct->keys[DIK_O] & 0x80)
//		worldPos.y += 4;
//	if(App.pdistruct->keys[DIK_L] & 0x80)
//		worldPos.y -= 4;


	// check for camera movement along the z-axis
	if(App.pdistruct->keys[DIK_Z] & 0x80)
		cameraFlags		= CAMERA_FLAG_ZOOM_IN | CAMERA_FLAG_CHECK_MOVE;
	if(App.pdistruct->keys[DIK_X] & 0x80)
		cameraFlags		= CAMERA_FLAG_ZOOM_OUT | CAMERA_FLAG_CHECK_MOVE;
//	if(App.pdistruct->keys[DIK_UP] & 0x80)
//		if((worldPos.z += 8) > 512)
//			worldPos.z	= 512;
//	if(App.pdistruct->keys[DIK_DOWN] & 0x80)
//		worldPos.z -= 4;


	// check for camera rotation about the x-axis
	if(App.pdistruct->keys[DIK_R] & 0x80)
		rotation.x	+= ( (rotation.x + 1) <= 359 ) ? 1 : -359;

	if(App.pdistruct->keys[DIK_T] & 0x80)
		rotation.x	-= ( (rotation.x - 1) >= 0   ) ? 1 : -359;


	// check for camera rotation about the y-axis
	if(App.pdistruct->keys[DIK_F] & 0x80)
		rotation.y	+= ( (rotation.y + 1) <= 359 ) ? 1 : -359;

	if(App.pdistruct->keys[DIK_G] & 0x80)
		rotation.y	-= ( (rotation.y - 1) >= 0   ) ? 1 : -359;


	// check for camera rotation about the z-axis
	if(App.pdistruct->keys[DIK_V] & 0x80)
		rotation.z	+= ( (rotation.z + 1) <= 359   ) ? 1 : -359;

	if(App.pdistruct->keys[DIK_B] & 0x80)
		rotation.z	-= ( (rotation.z - 1) >= 0   ) ? 1 : -359;



#endif
}
