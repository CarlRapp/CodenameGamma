#pragma once
#ifndef PLAYSCREEN_H
#define PLAYSCREEN_H

#include "..\Screen.h"
#include "Graphics\Effects.h"
#include "Graphics\RenderStates.h"
#include "Graphics\Buffer.h"
#include "Graphics\GraphicsManager.h"
#include "GameObject.h"
//#include "Player.h"
#include "Terrain\Terrain.h"
#include "Graphics\TextureManager.h"
#include "Level/Level.h"

class PlayScreen : public Screen
{
private:
	ScreenData*	gScreenData;

	Level*		gLevel;

protected:
	bool	Load();
	bool	Unload();
public:
	PlayScreen(ScreenData* Setup);

	void	Update(float DeltaTime);
	void	Render();
	void	SetNumberOfPlayers(int noPlayers) { if (gLevel) gLevel->SetNumberOfPlayers(noPlayers, gScreenWidth, gScreenHeight); }

	ScreenType	GetScreenType();

	void	Reset();
};

#endif
