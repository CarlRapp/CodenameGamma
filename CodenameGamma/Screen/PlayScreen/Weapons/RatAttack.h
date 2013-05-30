#pragma once
#ifndef RATATTACK_H
#define RATATTACK_H

#include "Weapon.h"

using namespace DirectX;

class RatAttack : public Weapon
{
public:
	RatAttack(void);
	~RatAttack(void);

	bool	IsDropable(){	return false;	}
	bool	Fire( GameObject* Owner, GameObject* Target, float DamageMul );
};

#endif