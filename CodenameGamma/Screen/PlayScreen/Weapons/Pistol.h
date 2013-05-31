#pragma once
#ifndef PISTOL_H
#define PISTOL_H

#include "Weapon.h"
#include "Projectiles/Bullet.h"

using namespace DirectX;

class Pistol : public Weapon
{
public:
	Pistol(void);
	~Pistol(void);

	bool	Fire( GameObject* Owner, GameObject* Target, float DamageMul );

	bool	IsDropable(){ return false; }
};

#endif