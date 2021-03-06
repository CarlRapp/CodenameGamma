#pragma once
#ifndef PREPLAYSCREEN_H
#define PREPLAYSCREEN_H

#include "MainMenuScreen.h"

class PrePlayScreen : public MainMenuScreen
{
private:
	ScreenData*	gScreenData;

	ID3D11ShaderResourceView*	gBackground;
	
	bool	gConfirmNext;

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