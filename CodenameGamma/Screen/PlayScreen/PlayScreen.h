#pragma once
#ifndef PLAYSCREEN_H
#define PLAYSCREEN_H

#include "..\Screen.h"
#include "Graphics\Effects.h"
#include "Graphics\RenderStates.h"
#include "Graphics\Buffer.h"
#include "Graphics\GraphicsManager.h"
#include "GameObject.h"
#include "Player.h"
#include "Terrain\Terrain.h"
#include "Graphics\TextureManager.h"

class PlayScreen : public Screen
{
private:

	TextureManager						gTexMgr;
	vector<Player*>						gPlayers;
	Model								*gModel;

	GraphicsManager						*gGraphicsManager;
	vector<GameObject*>					gGameObjects;
	Terrain								*gTerrain;
	StructuredBuffer<DirectionalLight>	*gDirLightBuffer;

	void AddInstance(float x, float y, float z, Model *model);

protected:
	bool	Load();
	bool	Unload();
public:
	PlayScreen(ScreenData* Setup);

	void	Update(float DeltaTime);
	void	Render();
	void	SetNumberOfPlayers(int noPlayers);

	ScreenType	GetScreenType();
};

#endif
