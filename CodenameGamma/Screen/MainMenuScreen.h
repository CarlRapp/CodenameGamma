#pragma once
#ifndef MAINMENUSCREEN_H
#define MAINMENUSCREEN_H

#include "Screen.h"

class MainMenuScreen : public Screen
{
protected:
	typedef pair<string, ScreenType>		MenuEntry;
	map <const int, MenuEntry>				gMenuEntries;
	map <const int, MenuEntry>::iterator	gMenuIterator;

	int		gCurrentIndex;
	float	gTextSize, gTextSizeActive, gTextSizeActiveTicker;

	bool	Load();
	bool	Unload();
public:
	MainMenuScreen(ScreenData* Setup);

	void	Update(float DeltaTime);
	void	Render();

	ScreenType	GetScreenType();
};
#endif