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

	vector<Weapon*>	gWeaponList;
	Weapon*			gCurrentWeapon;

	float gWalkSpeed;
	float gRunSpeed;
	
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
	void			SetWeapon(Weapon* Weapon);
	bool			HasWeapon() { return gCurrentWeapon != 0; }

	void			SetHealth(UnitHealth HealthData);
	UnitHealth		GetHealth();
	Weapon*			GetWeapon() { return gCurrentWeapon; }

	void SetMoveState(MoveState newMoveState);
	void SetWeaponState(WeaponState newWeaponState);

	bool CanRun() { return gMoveDirection == Forward; }
	bool Crouching() { return gMoveState == Crouch || PlayingAnimation("StandUp"); }

	void SwitchCrouchStand() { if (gMoveState == Crouch) SetMoveState(Stand); else SetMoveState(Crouch); }

	virtual	void	Hurt(float Damage);

	void	ReloadWeapon();
	void	FireWeapon();

	virtual bool Intersects(GameObject* B, vector<CollisionData>& CD);

	void	CollideWith(GameObject* Instance);


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

	virtual	void	Heal( float Value );
};

#endif