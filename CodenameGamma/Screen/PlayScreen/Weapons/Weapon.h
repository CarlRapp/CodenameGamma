#pragma once
#ifndef WEAPON_H
#define WEAPON_H

#include "../GameObject.h"
#include "Projectiles/Projectile.h"

using namespace DirectX;

//	Is arranged like <Current, Cooldown>
typedef pair<float, float>	WeaponCooldown;

//	Is arranged like <Current, Cooldown>
typedef pair<float, float>	WeaponReloadTime;

//	Ammo clip <Current, Max>
typedef pair<int, int>		WeaponClip;

enum WeaponState
{
	Ready,
	Reloading
};

class Weapon : public GameObject
{
protected:
	WeaponState			gState;
	WeaponCooldown		gCooldown;
	WeaponClip			gClip;
	WeaponReloadTime	gReloadTime;

	virtual	void	LowerCooldown(float DeltaTime);
	virtual	void	ReloadCountdown(float DeltaTime);
public:
	Weapon(void);
	~Weapon(void);

	//	This will be "master update"
	virtual void	Update(float deltaTime, Terrain* terrain);

	virtual	bool	CanFire() { return (gCooldown.first <= 0); }

	virtual void	Fire();

	virtual	void	Reload();
};

#endif