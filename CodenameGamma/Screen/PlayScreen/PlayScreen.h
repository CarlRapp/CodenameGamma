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

	bool	isPaused;
	int		gPauseScreenIndex;

	ID3D11ShaderResourceView*	gHealthBar[10];
	ID3D11ShaderResourceView*	gHungerBar[6];
	ID3D11ShaderResourceView*	gThirstBar[6];
	ID3D11ShaderResourceView*	gBackground;

	void	RenderGUI( Player* P );
	void	RenderGUISprite( D3D11_VIEWPORT VP, ID3D11ShaderResourceView* Sprite );
	void	RenderGUIText( XMFLOAT2 Position, string Text, float TextSize, TextColor Color );

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

	void	RenderPauseScreen();
};

#endif
