#pragma once
#ifndef MOUSE_H
#define MOUSE_H

#include "InputEnums.h"


//	DirectInput
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class Mouse
{
private:
	HWND*					gWindow;
	IDirectInput8*			gDirectInput;
	IDirectInputDevice8*	gMouseDevice;
	DIMOUSESTATE			gMouseState;
	bool*					gOldKeyState;
	bool*					gNewKeyState;

	XMFLOAT2				gMousePositionRelative, gMousePositionAbsolute;
	XMFLOAT2				gMouseMovement;
	int						gMouseScrollValue;

	LPRECT	gWindowRect;
	LPPOINT	gMousePoint;
	int		gScreenWidth, gScreenHeight;

	void	FixPosition();

public:
	Mouse(void);
	Mouse(HINSTANCE* HInstance, HWND* Hwnd, int ScreenWidth, int ScreenHeight);
	~Mouse(void);

	void	Update();

	InputState	GetButtonState(MouseButton Button);

	XMFLOAT2	GetPosition(bool RelativeWindow);
	XMFLOAT2	GetMovement();

	int			GetScrollValue();
};

#endif