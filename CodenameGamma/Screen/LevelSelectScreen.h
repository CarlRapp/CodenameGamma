#pragma once
#ifndef LEVELSELECTSCREEN_H
#define LEVELSELECTSCREEN_H

#include "Screen.h"
using namespace std;

class LevelSelectScreen : public Screen
{
private:
	struct	LevelInfo
	{
		LevelInfo(){};
		string	Name;
		int		Width, Height;
		int		PlayerCount;
	};

	ScreenData*	gScreenData;

	typedef pair<string, LevelInfo>		MapMenuEntry;
	vector<MapMenuEntry>				gMapMenu;
	int	gStartIndex;
	int	gListLimit;
	int	gCurrentIndex;
	int	gMenuTextSize;

	void	CreateMapMenu( void );

public:
	LevelSelectScreen(ScreenData* Setup);

	void	Update(float DeltaTime);
	void	Render();

	ScreenType	GetScreenType();
};
#endif