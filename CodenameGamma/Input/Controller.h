#pragma once
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "InputEnums.h"

class Controller
{
private:
	int		gIndex;
	float	gDeadZone;
	float	gVibrateTimer;

	DWORD			gIsConnected;
	XINPUT_STATE	gInputState;

	bool*	gOldButtonState;
	bool*	gNewButtonState;

public:
	Controller(void);
	Controller(int Index);
	~Controller(void);

	void	Update(float DeltaTime);

	float			GetTriggerValue(Xbox_Direction Trigger);
	InputState		GetButtonState(Xbox_Button Button);
	XMFLOAT2		GetStickDirection(Xbox_Direction Stick);
	float			GetStickLength(Xbox_Direction Stick);

	void	Vibrate(float Left, float Right, float Time);
	void	SetDeadZone(float Value);
	void	SetIndex(int Index);

	bool	IsConnected();
};

#endif