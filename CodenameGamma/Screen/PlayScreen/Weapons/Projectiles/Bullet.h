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

	void	CollideWith(GameObject* Instance);
};

#endif