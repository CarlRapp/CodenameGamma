#pragma once
#ifndef WEAPON_H
#define WEAPON_H

#include "../GameObject.h"
#include "Projectiles/Projectile.h"

using namespace DirectX;

//	Is arranged like <Current, Cooldown>
typedef pair<float, float>	WeaponCooldown;

class Weapon : public GameObject
{
protected:
	WeaponCooldown	gCooldown;


	virtual	void	LowerCooldown(float DeltaTime);
public:
	Weapon(void);
	~Weapon(void);

	//	This will be "master update"
	virtual bool Update(float deltaTime, Terrain* terrain);

	virtual	bool	CanFire() { return (gCooldown.first <= 0); }

	virtual vector<Projectile*> Fire();
};

#endif