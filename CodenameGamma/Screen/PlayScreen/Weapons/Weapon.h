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

struct WeaponAnimations
{
	string Draw;
	string Aim;
	string PutAway;
	string UpperStand;
	string UpperWalk;
	string UpperRun;
	string Shoot;
	string Reload;
	string DrawReloadPutAway;

	WeaponAnimations()
	{
		Aim					= "";
		Draw				= "";
		DrawReloadPutAway	= "";
		PutAway				= "";
		Shoot				= "";
		Reload				= "";
		UpperWalk			= "";
		UpperRun			= "";
		UpperStand			= "";
	}
};

class Weapon : public GameObject
{
protected:
	WeaponState			gState;
	WeaponCooldown		gCooldown;
	WeaponClip			gClip;
	WeaponReloadTime	gReloadTime;

	string				gReloadSound;

	virtual	void	LowerCooldown(float DeltaTime);
	virtual	void	ReloadCountdown(float DeltaTime);
public:
	Weapon(void);
	~Weapon(void);

	WeaponAnimations gWeaponAnimations;

	//	This will be "master update"
	virtual void	Update(float deltaTime, Terrain* terrain);

	virtual	bool	CanFire() { return (gCooldown.first <= 0 && gClip.first > 0 && gState == Ready); }

	virtual bool	Fire( GameObject* Owner, GameObject* Target, float DamageMul );

	bool			NeedReload() { return gClip.first <= 0 && gState != Reloading; }

	virtual	bool	Reload();

	virtual bool	Intersects(GameObject* B, vector<CollisionData>& CD);
	virtual	bool	IsDropable();

	float			GetReloadTime() { return gReloadTime.second; }

	/*
	To make it easier for the GUI
	to get info about a weapon, this
	will be returned when calling
	GetInfo()
	*/
	struct WeaponInfo
	{
		WeaponCooldown		Cooldown;
		WeaponClip			Magazine;
		WeaponReloadTime	ReloadTime;

		WeaponInfo( Weapon* Instance )
		{
			Cooldown	=	Instance->gCooldown;
			Magazine	=	Instance->gClip;
			ReloadTime	=	Instance->gReloadTime;
		}
	};
	WeaponInfo	GetInfo()
	{
		return	WeaponInfo( this );
	}
};

#endif