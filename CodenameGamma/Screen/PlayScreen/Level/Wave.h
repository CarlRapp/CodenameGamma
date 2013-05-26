#pragma once
#ifndef WAVE_H
#define WAVE_H

#include "../../../AI/Patrol/NodeMap.h"
#include "../GameObject.h"


typedef pair<float, float>	WaveTimer;

class Wave
{
private:
	GameObjectCallback	AddGameObject;

	NodeMap*	gNodeMap;
	WaveTimer	gWaveTimer;
	float		gHealthMultiplier, gSpeedMultiplier, gDamageMultiplier;

	void		NewWave(void);
public:
	Wave(void);
	Wave(NodeMap* Instance);
	~Wave(void);

	void	Update(float DeltaTime);

	void	SetAddGameObject(GameObjectCallback callback) { AddGameObject = callback; }
	float	GetCountdown(){ return gWaveTimer.first; }
};

#endif