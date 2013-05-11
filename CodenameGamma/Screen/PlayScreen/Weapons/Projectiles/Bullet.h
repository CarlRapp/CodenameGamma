#pragma once
#ifndef BULLET_H
#define BULLET_H

#include "Projectile.h"

using namespace DirectX;

class Bullet : public Projectile
{
public:
	Bullet(void);
	~Bullet(void);

	virtual	 void	CollideWith(GameObject* Instance, vector<CollisionData> CD);

	float	GetSpeed();
};

#endif