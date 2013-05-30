#pragma once
#ifndef GHOSTATTACK_H
#define GHOSTATTACK_H

#include "Weapon.h"
#include "Projectiles\Orb.h"

using namespace DirectX;

class GhostAttack : public Weapon
{
public:
	GhostAttack(void);
	~GhostAttack(void);

	bool	IsDropable(){	return false;	}
	bool	Fire( GameObject* Owner, GameObject* Target, float DamageMul );
};

#endif