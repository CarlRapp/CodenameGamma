#include "Mouse.h"

Mouse::Mouse(){}
Mouse::Mouse(HINSTANCE* HInstance, HWND* Hwnd, int ScreenWidth, int ScreenHeight)
{
	gWindow		=	Hwnd;
	gWindowRect	=	new RECT();
	gMousePoint	=	new POINT();

	GetWindowRect(*gWindow, gWindowRect);

	gScreenWidth	=	(int)((gWindowRect->left + gWindowRect->right));
	gScreenHeight	=	(int)((gWindowRect->top + gWindowRect->bottom));

	gMousePositionRelative.x	=	0;
	gMousePositionRelative.y	=	0;
	gMousePositionAbsolute.x	=	0;
	gMousePositionAbsolute.y	=	0;
	gMouseMovement.x	=	0;
	gMouseMovement.y	=	0;
	gMouseScrollValue	=	0;

	gNewKeyState	=	new bool[4];
	gOldKeyState	=	new bool[4];

	// Initialize the main direct input interface.
	if(FAILED(DirectInput8Create(*HInstance, 0x0800, IID_IDirectInput8, (void**)&gDirectInput, NULL)))
		return;

	// Initialize the direct input interface for the mouse.
	if(FAILED(gDirectInput->CreateDevice(GUID_SysMouse, &gMouseDevice, NULL)))
		return;

	// Set the data format for the mouse using the pre-defined mouse data format.
	if(FAILED(gMouseDevice->SetDataFormat(&c_dfDIMouse)))
		return;

	// Set the cooperative level of the mouse to share with other programs.
	if(FAILED(gMouseDevice->SetCooperativeLevel(*Hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		return;

	// Acquire the mouse.
	if(FAILED(gMouseDevice->Acquire()))
		return;

	gCenterMouse	=	false;
}

Mouse::~Mouse()
{
	if( gMouseDevice )
	{
		gMouseDevice->Unacquire();
		gMouseDevice->Release();
		gMouseDevice	=	0;
	}

	if( gDirectInput )
	{
		gDirectInput->Release();
		gDirectInput	=	0;
	}

	delete	gNewKeyState;
	gNewKeyState	=	0;
	delete	gOldKeyState;
	gOldKeyState	=	0;
	
	delete	gWindowRect;
	gWindowRect	=	0;
	delete	gMousePoint;
	gMousePoint	=	0;
}

void Mouse::Update()
{
	HRESULT tResult;
	// Read the mouse device.
	tResult	=	gMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&gMouseState);

	if(FAILED(tResult))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((tResult == DIERR_INPUTLOST) || (tResult == DIERR_NOTACQUIRED))
			gMouseDevice->Acquire();
		else
			return;
	}

	for(int i = 0; i < 4; ++i)
	{
		gOldKeyState[i]	=	gNewKeyState[i];

		gNewKeyState[i]	=	(gMouseState.rgbButtons[i] & 0x80) == 0x80;
	}

	FixPosition();

	gMouseScrollValue	+=	gMouseState.lZ;
	if( gMouseScrollValue != 0 )
		gMouseScrollValue	/=	120;
}

void Mouse::FixPosition()
{
	GetWindowRect(*gWindow, gWindowRect);
	GetCursorPos(gMousePoint);

	int centerX		=	(int)((gWindowRect->left + gWindowRect->right) * 0.5f);
	int centerY		=	(int)((gWindowRect->top + gWindowRect->bottom) * 0.5f);

	if(gCenterMouse)
	{
		gMousePositionAbsolute.x	=	1.0f * centerX;
		gMousePositionAbsolute.y	=	1.0f * centerY;

		gMouseMovement.x	=	(float)(gMousePoint->x - centerX);
		gMouseMovement.y	=	(float)(centerY - gMousePoint->y);
		SetCursorPos(centerX, centerY);
	}
	else
	{
		gMouseMovement.x	=	(float)(gMousePoint->x - gMousePositionAbsolute.x);
		gMouseMovement.y	=	(float)(gMousePoint->y - gMousePositionAbsolute.y);

		gMousePositionAbsolute.x	+=	gMouseMovement.x;
		gMousePositionAbsolute.y	+=	gMouseMovement.y;

		int	minX	=	(int)gWindowRect->left;
		int	maxX	=	(int)gWindowRect->right;
		int	minY	=	(int)gWindowRect->top;
		int	maxY	=	(int)gWindowRect->bottom;
		
		//	Clamp the mouse inside the window
		gMousePositionAbsolute.x	=	( gMousePositionAbsolute.x < minX ) ? minX : gMousePositionAbsolute.x;
		gMousePositionAbsolute.x	=	( gMousePositionAbsolute.x > maxX ) ? maxX : gMousePositionAbsolute.x;
		gMousePositionAbsolute.y	=	( gMousePositionAbsolute.y < minY ) ? minY : gMousePositionAbsolute.y;
		gMousePositionAbsolute.y	=	( gMousePositionAbsolute.y > maxY ) ? maxY : gMousePositionAbsolute.y;

		gMousePositionRelative.x	=	gMousePositionAbsolute.x - minX;
		gMousePositionRelative.y	=	gMousePositionAbsolute.y - minY;

		SetCursorPos((int)gMousePositionAbsolute.x, (int)gMousePositionAbsolute.y);
	}
}

void Mouse::SetClampCenter(bool Value)
{
	gCenterMouse	=	Value;
}

XMFLOAT2 Mouse::GetPosition(bool RelativeWindow)
{
	return RelativeWindow ? gMousePositionRelative : gMousePositionAbsolute;
}

XMFLOAT2 Mouse::GetMovement()
{
	return gMouseMovement;
}

InputState Mouse::GetButtonState(MouseButton Button)
{
	bool	tOld	=	gOldKeyState[Button];
	bool	tNew	=	gNewKeyState[Button];

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

int Mouse::GetScrollValue()
{
	return gMouseScrollValue;
}