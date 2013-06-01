#pragma once
#ifndef AUTOMATICRIFLE_H
#define AUTOMATICRIFLE_H

#include "Weapon.h"
#include "Projectiles/Bullet.h"

using namespace DirectX;

class AutomaticRifle : public Weapon
{
public:
	AutomaticRifle(void);
	~AutomaticRifle(void);

	bool	CanFire();

	bool	Fire( GameObject* Owner, GameObject* Target, float DamageMul );
};

#endif