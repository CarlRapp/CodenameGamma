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

	bool			isPaused;
	int				gPauseScreenIndex;
	vector<string>	gPauseScreenEntry;

	ID3D11ShaderResourceView*	gHealthBar[10];
	ID3D11ShaderResourceView*	gHungerBar[6];
	ID3D11ShaderResourceView*	gThirstBar[6];
	ID3D11ShaderResourceView*	gBackground;
	ID3D11ShaderResourceView*	gBulletGUI;
	ID3D11ShaderResourceView*	gWaveBackground;
	ID3D11ShaderResourceView*	gSpectatingBackground;

	ID3D11ShaderResourceView*	gWeapons[4];
	D3D11_VIEWPORT				gWeaponsVP[4];
	IFW1FontWrapper*			gWaveTextWrapper;

	void	RenderGUI( Player* P );
	void	RenderGUISprite( D3D11_VIEWPORT VP, ID3D11ShaderResourceView* Sprite );
	void	RenderGUIText( XMFLOAT2 Position, string Text, float TextSize, TextColor Color );
	void	RenderGUIText( IFW1FontWrapper* FontWrapper, XMFLOAT2 Position, string Text, float TextSize, TextColor Color );
	void	RenderGUISplitter();

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
