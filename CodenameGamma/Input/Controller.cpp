#include "Controller.h"

Controller::Controller()
{
	gIndex		=	0;
	gDeadZone	=	0.3f;

	gOldButtonState	=	new bool[NUMBER_OF_BUTTONS];
	gNewButtonState	=	new bool[NUMBER_OF_BUTTONS];
}
Controller::Controller(int Index)
{
	gIndex		=	Index;
	gDeadZone	=	0.3f;

	gOldButtonState	=	new bool[NUMBER_OF_BUTTONS];
	gNewButtonState	=	new bool[NUMBER_OF_BUTTONS];
}

Controller::~Controller()
{
	gOldButtonState	=	0;
	gNewButtonState	=	0;
}

void Controller::Update()
{
	ZeroMemory( &gInputState, sizeof(XINPUT_STATE) );
	gIsConnected	=	XInputGetState( gIndex, &gInputState );

	if( !IsConnected() )
		return;

	for(int i = 0; i < NUMBER_OF_BUTTONS; ++i)
		gOldButtonState[i]	=	gNewButtonState[i];

	gNewButtonState[A]	=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)	==	XINPUT_GAMEPAD_A;
	gNewButtonState[B]	=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_B)	==	XINPUT_GAMEPAD_B;
	gNewButtonState[X]	=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_X)	==	XINPUT_GAMEPAD_X;
	gNewButtonState[Y]	=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)	==	XINPUT_GAMEPAD_Y;

	gNewButtonState[START]	=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START)	==	XINPUT_GAMEPAD_START;
	gNewButtonState[BACK]	=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)	==	XINPUT_GAMEPAD_BACK;

	gNewButtonState[D_UP]		=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)		==	XINPUT_GAMEPAD_DPAD_UP;
	gNewButtonState[D_DOWN]		=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)	==	XINPUT_GAMEPAD_DPAD_DOWN;
	gNewButtonState[D_LEFT]		=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)	==	XINPUT_GAMEPAD_DPAD_LEFT;
	gNewButtonState[D_RIGHT]	=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)	==	XINPUT_GAMEPAD_DPAD_RIGHT;

	gNewButtonState[LEFT_BUMPER]	=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)	==	XINPUT_GAMEPAD_LEFT_SHOULDER;
	gNewButtonState[RIGHT_BUMPER]	=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)	==	XINPUT_GAMEPAD_RIGHT_SHOULDER;

	gNewButtonState[LEFT_STICK]		=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)	==	XINPUT_GAMEPAD_LEFT_THUMB;
	gNewButtonState[RIGHT_STICK]	=	(gInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)	==	XINPUT_GAMEPAD_RIGHT_THUMB;
}

float Controller::GetTriggerValue(Xbox_Direction Trigger)
{
	float	tValue;

	switch(Trigger)
	{
	case LEFT:
		tValue	=	(float)gInputState.Gamepad.bLeftTrigger;
		break;
	case RIGHT:
		tValue	=	(float)gInputState.Gamepad.bRightTrigger;
		break;
	}

	return tValue / 255.0f;
}

InputState Controller::GetButtonState(Xbox_Button Button)
{
	bool	tOld	=	gOldButtonState[Button];
	bool	tNew	=	gNewButtonState[Button];

	//	If the button was pressed this frame
	//	but NOT the last frame
	if(!tOld && tNew)
		return	PRESSED;

	//	If the button was pressed last frame
	//	AND this frame
	else if(tOld && tNew)
		return	DOWN;

	//	If the button was pressed last frame
	//	but NOT this frame
	else if(tOld && !tNew)
		return	RELEASED;

	//	If no other state was found the
	//	button must be up.
	return	UP;
}

void Controller::Vibrate(float Left, float Right)
{
	//	Clamp the values from 0 to 1
	Left	=	max(min(Left, 1), 0);
	Right	=	max(min(Right, 1), 0);

    // Create a Vibraton State
    XINPUT_VIBRATION Vibration;

    // Zeroise the Vibration
    ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

    // Set the Vibration Values
    Vibration.wLeftMotorSpeed	=	(WORD)(Left * 65535);
    Vibration.wRightMotorSpeed	=	(WORD)(Right * 65535);

    // Vibrate the controller 
    XInputSetState(gIndex, &Vibration);
}

D3DXVECTOR2 Controller::GetStickDirection(Xbox_Direction Stick)
{
	if(GetStickLength(Stick) ==	0)
		return D3DXVECTOR2(0, 0);

	D3DXVECTOR2	tVec;

	if(Stick == LEFT)
	{
		tVec.x	=	gInputState.Gamepad.sThumbLX;
		tVec.y	=	gInputState.Gamepad.sThumbLY;
	}
	else
	{
		tVec.x	=	gInputState.Gamepad.sThumbRX;
		tVec.y	=	gInputState.Gamepad.sThumbRY;
	}
	tVec	/=	32767;

	tVec.x	=	(tVec.x < -1) ? -1 : tVec.x;
	tVec.x	=	(tVec.x > 1) ? 1 : tVec.x;

	tVec.y	=	(tVec.y < -1) ? -1 : tVec.y;
	tVec.y	=	(tVec.y > 1) ? 1 : tVec.y;
	
	return tVec;
}

float Controller::GetStickLength(Xbox_Direction Stick)
{
	float	tX, tY, tLength;

	if(Stick == LEFT)
	{
		tX	=	gInputState.Gamepad.sThumbLX;
		tY	=	gInputState.Gamepad.sThumbLY;
	}
	else
	{
		tX	=	gInputState.Gamepad.sThumbRX;
		tY	=	gInputState.Gamepad.sThumbRY;
	}

	tLength	=	max(min(sqrt(tX*tX + tY*tY) / 32767, 1), 0);

	return (tLength <= gDeadZone) ? 0 : tLength;
}

void Controller::SetDeadZone(float Value)
{
	gDeadZone	=	max(min(Value, 1), 0);
}

void Controller::SetIndex(int Index)
{
	if(Index < 0 || Index >= 4)
		return;

	gIndex	=	Index;
}

bool Controller::IsConnected()
{
	return (gIsConnected == ERROR_SUCCESS);
}