#pragma once
#ifndef PLAYSCREEN_H
#define PLAYSCREEN_H

#include "Screen\Screen.h"

class PlayScreen : public Screen
{
protected:
	bool	Load();
	bool	Unload();
public:
	PlayScreen(ScreenData* Setup);

	void	Update(float DeltaTime);
	void	Render();

	ScreenType	GetScreenType();
};

#endif
