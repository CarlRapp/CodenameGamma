#pragma once
#ifndef RAT_H
#define RAT_H

#include "EnemyUnit.h"

using namespace DirectX;

class Rat : public EnemyUnit
{
public:
	Rat(void);
	~Rat(void);

	void	Update(float deltaTime, Terrain* terrain);
};

#endif