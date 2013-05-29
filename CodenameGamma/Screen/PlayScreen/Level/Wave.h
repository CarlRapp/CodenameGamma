#pragma once
#ifndef WAVE_H
#define WAVE_H

#include "../../../AI/Patrol/NodeMap.h"
#include "../GameObject.h"


typedef pair<float, float>	WaveTimer;
typedef pair<int, int>		WaveUnits;

struct WaveInfo
{
	int		Rats, Tanks, Ghosts;
	float	RatInfo[3], TankInfo[3], GhostInfo[3];
	WaveInfo()
	{
		Rats	=	0;
		Tanks	=	0;
		Ghosts	=	0;

		for( int n = 0; n < 3; ++n )
		{
			RatInfo[n]		=	1.0f;
			TankInfo[n]		=	1.0f;
			GhostInfo[n]	=	1.0f;
		}
	}
};

class Wave
{
private:

	GameObjectCallback	AddGameObject;

	NodeMap*	gNodeMap;
	WaveTimer	gWaveTimer;
	float		gHealthMultiplier, gSpeedMultiplier, gDamageMultiplier;
	WaveUnits	gUnitsSpawned;

	vector<WaveInfo>	gWaves;
	int					gCurrentWave;

	void	NewWave(void);
	void	MoveUnit( GameObject* Instance );

	WaveInfo	ReadWaveLine( string Line );
public:
	Wave(void);
	Wave(NodeMap* Instance);
	~Wave(void);

	void	Update(float DeltaTime);

	void	SetAddGameObject(GameObjectCallback callback) { AddGameObject = callback; }
	float	GetCountdown(){ return gWaveTimer.first; }
	bool	IsLimitReached(){ return gUnitsSpawned.first == gUnitsSpawned.second; }

	void	UnitKilled(){ --gUnitsSpawned.first; }

	void	LoadWaveData( string LevelPath );
};

#endif