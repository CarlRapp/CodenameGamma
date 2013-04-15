#pragma once
#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "../stdafx.h"
#include "InputEnums.h"
#include "Controller.h"
#include "Keyboard.h"
#include "Mouse.h"

using namespace std;

class InputManager
{
private:
	static	InputManager*	gInstance;
	InputManager(void);
	InputManager(HINSTANCE* HInstance, HWND* Hwnd, int ScreenWidth, int ScreenHeight);

	HWND*					gWindow;
	vector<Controller*>*	gControllers;
	Keyboard*				gKeyboard;
	Mouse*					gMouse;
	

public:
	~InputManager(void);

	static	InputManager*	GetInstance();
	static	void			Initialize(HINSTANCE* HInstance, HWND* Hwnd, int ScreenWidth, int ScreenHeight);


	void	Update();

	Controller*	GetController(int Index);
	Keyboard*	GetKeyboard();
	Mouse*		GetMouse();
};

#endif