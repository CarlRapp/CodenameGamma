#pragma once
#ifndef POSTPLAYSCREEN_H
#define POSTPLAYSCREEN_H

#include "Screen.h"

class PostPlayScreen : public Screen
{
private:
	ScreenData*	gScreenData;

	ID3D11ShaderResourceView*	gBackground;
public:
	PostPlayScreen(ScreenData* Setup);
	bool	Load();
	bool	Unload();
	void	Update(float DeltaTime);
	void	Render();

	ScreenType	GetScreenType();
};
#endif