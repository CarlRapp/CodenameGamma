#pragma once
#ifndef ORB_H
#define ORB_H

#include "Projectile.h"

using namespace DirectX;

class Orb : public TargetProjectile
{
public:
	Orb(void);
	~Orb(void);

	void	Update(float deltaTime, Terrain* terrain);

	float	GetSpeed();
};

#endif