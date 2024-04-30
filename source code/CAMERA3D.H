#ifndef camera_h
#define camera_h




#define CAMERA_FLAG_MOVE_UP			0x1
#define CAMERA_FLAG_MOVE_DOWN		0x2
#define CAMERA_FLAG_MOVE_LEFT		0x4
#define CAMERA_FLAG_MOVE_RIGHT		0x8
#define CAMERA_FLAG_ZOOM_IN			0x10
#define CAMERA_FLAG_ZOOM_OUT		0x20

#define CAMERA_FLAG_CHECK_MOVE		0x40

#define CAMERA_FLAG_MOVEMENT_FLAGS	0x3F




class Camera3D
{
public:
	int			cameraFlags,
				moveDirection;

	Vector3D	worldPos,		// the camera's position (in world coordinates)
				rotation,		// the camera's angles of rotation about the x-, y-, and z-axes
				headlight,		// unit vector which is perpendicular to line of sight
				targetPos,
				velocity;

	// member functions
	Camera3D();

	void	Update();
	void	ResetPosition() {cameraFlags = moveDirection = 0; worldPos.x = worldPos.y = worldPos.z = 0; targetPos = worldPos;}
	void	GetDirectionVector();
	void	HandleKeyInput();

	int		IsMoving()						{ return (cameraFlags & CAMERA_FLAG_MOVEMENT_FLAGS); }
	void	HandleMovement();
	void	UpdateVeloc();
	void	UpdatePos();
	void	SetMovementDirection(int direction)
			{
				switch(direction)
				{
				case 0:	cameraFlags	= CAMERA_FLAG_MOVE_UP	 | CAMERA_FLAG_CHECK_MOVE; break;
				case 1:	cameraFlags	= CAMERA_FLAG_MOVE_DOWN	 | CAMERA_FLAG_CHECK_MOVE; break;
				case 2:	cameraFlags	= CAMERA_FLAG_MOVE_LEFT	 | CAMERA_FLAG_CHECK_MOVE; break;
				case 3:	cameraFlags	= CAMERA_FLAG_MOVE_RIGHT | CAMERA_FLAG_CHECK_MOVE; break;
				case 4:	cameraFlags	= CAMERA_FLAG_ZOOM_IN	 | CAMERA_FLAG_CHECK_MOVE; break;
				case 5:	cameraFlags	= CAMERA_FLAG_ZOOM_OUT	 | CAMERA_FLAG_CHECK_MOVE; break;
				default: break;
				}
			}
};




#endif