#pragma once
#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H
#include "stdafx.h"
#include "Screen.h"
#include "DebugScreen.h"
#include "MainMenuScreen.h"
using namespace std;



class ScreenManager
{
private:
	DebugScreen*		gDebugScreen;
	ScreenData*			gScreenData;

	vector<Screen*>*	gLoadedScreens;
	Screen*				gCurrentScreen;


	Screen*	LoadScreen(ScreenType Type);

	bool	gExit, gShowDebug;
	void	ToggleDebug();

public:
	ScreenManager(void);
	ScreenManager(ScreenData* Setup);
	~ScreenManager(void);

	void	ChangeScreen(ScreenType Type);
	void	Update(float DeltaTime);
	void	Render();

	bool	CloseGame();
	
};

#endif