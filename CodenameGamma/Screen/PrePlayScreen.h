#pragma once
#ifndef PREPLAYSCREEN_H
#define PREPLAYSCREEN_H

#include "MainMenuScreen.h"

class PrePlayScreen : public MainMenuScreen
{
private:
	ScreenData*	gScreenData;
public:
	PrePlayScreen(ScreenData* Setup);

	void	Update(float DeltaTime);
	void	Render();

	ScreenType	GetScreenType();
};
#endif