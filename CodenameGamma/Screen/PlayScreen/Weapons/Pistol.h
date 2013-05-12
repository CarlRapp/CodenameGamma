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

	bool	CanFire();

	void	Fire();
};

#endif