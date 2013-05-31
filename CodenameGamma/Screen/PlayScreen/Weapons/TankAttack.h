#pragma once
#ifndef TANKATTACK_H
#define TANKATTACK_H

#include "Weapon.h"

using namespace DirectX;

class TankAttack : public Weapon
{
public:
	TankAttack(void);
	~TankAttack(void);

	bool	IsDropable(){	return false;	}
	bool	Fire( GameObject* Owner, GameObject* Target, float DamageMul );
};

#endif