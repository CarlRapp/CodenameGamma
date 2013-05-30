#pragma once
#ifndef GHOST_H
#define GHOST_H

#include "EnemyUnit.h"

using namespace DirectX;

class Ghost : public EnemyUnit
{
public:
	Ghost(void);
	~Ghost(void);

	void	Update(float deltaTime, Terrain* terrain);
};

#endif