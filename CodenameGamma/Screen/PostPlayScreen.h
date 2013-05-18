#pragma once
#ifndef POSTPLAYSCREEN_H
#define POSTPLAYSCREEN_H

#include "Screen.h"

class PostPlayScreen : public Screen
{
private:
	ScreenData*	gScreenData;

public:
	PostPlayScreen(ScreenData* Setup);

	void	Update(float DeltaTime);
	void	Render();

	ScreenType	GetScreenType();
};
#endif