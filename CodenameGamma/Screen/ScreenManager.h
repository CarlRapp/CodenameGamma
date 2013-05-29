#pragma once
#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H
#include "../stdafx.h"
#include "Screen.h"
#include "MainMenuScreen.h"
#include "PlayScreen\PlayScreen.h"
#include "PlayScreen/Graphics/GraphicsManager.h"
using namespace std;



class ScreenManager
{
private:
	D3D11_VIEWPORT		gViewPort;
	ScreenData*			gScreenData;

	vector<Screen*>*	gLoadedScreens;
	Screen*				gCurrentScreen;
	GraphicsManager*	gGraphicsManager;


	Screen*	LoadScreen(ScreenType Type);

	bool	gExit, gShowDebug;
	void	ToggleDebug();

	DebugData*			gFpsCounter;

public:
	ScreenManager(void);
	ScreenManager(ScreenData* Setup, GraphicsManager* Instance);
	~ScreenManager(void);

	void	ChangeScreen(ScreenType Type);
	void	Update(float DeltaTime);
	void	Render();

	bool	CloseGame();
	
};

#endif