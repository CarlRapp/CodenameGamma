#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "InputEnums.h"

class Keyboard
{
private:
	bool*	gOldFrameKeyboard;
	bool*	gNewFrameKeyboard;
	int		gKeyboardTotalKeys;

public:
	Keyboard(void);
	~Keyboard(void);

	void	Update();

	InputState	GetKeyState(char Key);
};

#endif