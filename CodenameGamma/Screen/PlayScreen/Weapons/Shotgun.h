#pragma once
#ifndef SHOTGUN_H
#define SHOTGUN_H

#include "Weapon.h"
#include "Projectiles/Bullet.h"

using namespace DirectX;

#define NumBullets 10
#define MaxSpread 20.0f

class Shotgun : public Weapon
{
public:
	Shotgun(void);
	~Shotgun(void);

	bool	CanFire();

	bool	Fire( GameObject* Owner, GameObject* Target, float DamageMul );
};

#endif