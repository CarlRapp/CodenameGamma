#pragma once
#ifndef PREPLAYSCREEN_H
#define PREPLAYSCREEN_H

#include "MainMenuScreen.h"

class PrePlayScreen : public MainMenuScreen
{
private:
	ScreenData*	gScreenData;

	bool	Load();
	bool	Unload();
public:
	PrePlayScreen(ScreenData* Setup);

	void	Update(float DeltaTime);
	void	Render();
	void	Reset();

	ScreenType	GetScreenType();
};
#endif