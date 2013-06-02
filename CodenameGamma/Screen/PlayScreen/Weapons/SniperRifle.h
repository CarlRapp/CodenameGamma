#pragma once
#ifndef SNIPERRIFLE_H
#define SNIPERRIFLE_H

#include "Weapon.h"
#include "Projectiles/Bullet.h"

using namespace DirectX;

class SniperRifle : public Weapon
{
public:
	SniperRifle(void);
	~SniperRifle(void);

	bool	CanFire();

	bool	Fire( GameObject* Owner, GameObject* Target, float DamageMul );
};

#endif