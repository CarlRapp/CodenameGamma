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

	float	GetSpeed() { return 700.0f; }
};

#endif