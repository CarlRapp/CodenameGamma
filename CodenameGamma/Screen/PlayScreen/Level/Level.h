#pragma once
#ifndef LEVEL_H
#define LEVEL_H

#include "../../../stdafx.h"
#include "../Terrain/Terrain.h"

struct LevelData
{
	LevelData(){}

	ID3D11Device*			DEVICE;
	ID3D11DeviceContext*	DEVICE_CONTEXT;
};

class Level
{
private:
	Terrain*	gTerrain;

public:
	Level(void);
	Level(LevelData Data);
	~Level(void);

	void	Update(float DeltaTime);
	void	Render();


	Terrain*	GetTerrain();
};

#endif