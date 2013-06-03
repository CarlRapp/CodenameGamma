#pragma once
#ifndef CREDITSSCREEN_H
#define CREDITSSCREEN_H

#include "Screen.h"
using namespace std;

class CreditsScreen : public Screen
{
private:
	ID3D11ShaderResourceView*	gBackground;
	IFW1FontWrapper*			gWaveTextWrapper;

	bool	Load();
	bool	Unload();
public:
	CreditsScreen( ScreenData* Setup );

	void	Update( float DeltaTime );
	void	Render();

	ScreenType	GetScreenType();
};
#endif