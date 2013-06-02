#pragma once
#ifndef WEAPON_H
#define WEAPON_H

#include "../GameObject.h"
#include "WeaponFire.h"
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
	Reloading,
	Empty
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
	int					gAmmo;
	string				gReloadSound;
	int					gReloadSoundIndex;

	ModelInstance*		gWOGModel;

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

	bool			IsReloading() { return gState == Reloading; }

	void			SetWOGModelInstance(ModelInstance* modelInstance) 
	{ 
		SAFE_DELETE(gWOGModel);
		gWOGModel = modelInstance; 		
	}

	ModelInstance*	GetWOGModelInstance() { return gWOGModel; }

	XMFLOAT4		GetWeaponOffsetRotation();

	float			GetReloadTime() { return gReloadTime.second; }
	void			CancelReload()
	{
		SoundManager::GetInstance()->Stop( gReloadSoundIndex );
		gClip.first			=	0;
		gReloadTime.first	=	gReloadTime.second;
		gState				=	Ready;
	}

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
		int					Ammo;
		WeaponReloadTime	ReloadTime;

		WeaponInfo( Weapon* Instance )
		{
			Cooldown	=	Instance->gCooldown;
			Magazine	=	Instance->gClip;
			ReloadTime	=	Instance->gReloadTime;
			Ammo		=	Instance->gAmmo;
		}
	};
	WeaponInfo	GetInfo()
	{
		return	WeaponInfo( this );
	}
};

#endif