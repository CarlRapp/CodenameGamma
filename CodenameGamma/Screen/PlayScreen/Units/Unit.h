#pragma once
#ifndef UNIT_H
#define UNIT_H

#include "../GameObject.h"
#include "../Weapons/Weapon.h"

using namespace DirectX;

//	Is arranged like <Current, Max>
typedef pair<float, float>	UnitHealth;

class Unit : public GameObject
{
public:
	enum MoveState { Stand, Walk, Run, Crouch };
	enum WeaponState { Hold, Aim };

protected:

	MoveState	gMoveState;
	WeaponState gWeaponState;

	UnitHealth	gHealth;

	Weapon*	gPrimaryWeapon;
	Weapon*	gSecondaryWeapon;
	Weapon*	gCurrentWeapon;

	string	gDeathSound;

	float gWalkSpeed;
	float gRunSpeed;

	//	0 = Health, 1 = Damage, 2 = Speed
	float	gMultipliers[3];

	void	Boost()
	{
		//	Update health
		gHealth.second	*=	gMultipliers[0];
		gHealth.first	=	gHealth.second;

		//	Update health
		gHealth.second	*=	gMultipliers[0];

		//	Update speed
		gWalkSpeed	*=		gMultipliers[2];
		gRunSpeed	*=		gMultipliers[2];
	}
	
	string GetAnimation(string name)
	{
		if (gCurrentWeapon)
		{
			if (name == "Aim")		
				return gCurrentWeapon->gWeaponAnimations.Aim;

			else if (name == "Draw")		
				return gCurrentWeapon->gWeaponAnimations.Draw;
		
			else if (name == "DrawReloadPutAway")		
				return gCurrentWeapon->gWeaponAnimations.DrawReloadPutAway;
		
			else if (name == "PutAway")		
				return gCurrentWeapon->gWeaponAnimations.PutAway;
		
			else if (name == "Shoot")		
				return gCurrentWeapon->gWeaponAnimations.Shoot;
		
			else if (name == "Reload")		
				return gCurrentWeapon->gWeaponAnimations.Reload;

			else if (name == "UpperStand")
				return gCurrentWeapon->gWeaponAnimations.UpperStand;
		
			else if (name == "UpperWalk")		
				return gCurrentWeapon->gWeaponAnimations.UpperWalk;

			else if (name == "UpperRun")
				return gCurrentWeapon->gWeaponAnimations.UpperRun;
		}

		return "";
	}

public:
	Unit(void);
	~Unit(void);

	void	Update(float deltaTime, Terrain* terrain);
	void	UpdateAnimation();

	virtual void	SetTeam(GOTeam Value) { GameObject::SetTeam(Value); if ( gCurrentWeapon ) gCurrentWeapon->SetTeam(GetTeam()); }

	void			DropWeapon();

	void			SetHealth(UnitHealth HealthData);
	UnitHealth		GetHealth();
	Weapon*			GetWeapon() { return gCurrentWeapon; }

	void SetMoveState(MoveState newMoveState);
	void SetWeaponState(WeaponState newWeaponState);

	bool CanRun() { return gMoveDirection == Forward; }
	bool Crouching() { return gMoveState == Crouch || PlayingAnimation("StandUp"); }

	bool IsAiming() { return gWeaponState == Aim; }

	void SwitchCrouchStand() { if (gMoveState == Crouch) SetMoveState(Stand); else SetMoveState(Crouch); }

	virtual	void	Hurt(float Damage);

	void	ReloadWeapon();
	bool	FireWeapon(GameObject* Target);
	void	ChangeWeapon();
	void	PickupWeapon( Weapon* Instance );
	bool	HasSecondaryWeapon() { return gSecondaryWeapon != NULL; }

	virtual bool Intersects(GameObject* B, vector<CollisionData>& CD);

	void	CollideWith(GameObject* Instance);

	bool	IsRunning() { return gMoveState == Run; }

	float	GetSpeed()
	{
		float	speed;
		XMStoreFloat(
			&speed,
			XMVector3Length( XMLoadFloat3( &GetFloat3Value( Velocity ) ) )
		);

		return speed * MeterPerUnits;
	}

	float GetWalkSpeed() { return gWalkSpeed; }
	float GetRunSpeed() { return gRunSpeed; }

	void	SetMultipliers( float Health, float Damage, float Speed )
	{
		gMultipliers[0]	=	Health;
		gMultipliers[1]	=	Damage;
		gMultipliers[2]	=	Speed;
	}
	void	Boost( int NumberOfTimes )
	{
		for( int n = 0; n < NumberOfTimes; ++n )
			Boost();
	}

	virtual	void	Heal( float Value );
};

#endif