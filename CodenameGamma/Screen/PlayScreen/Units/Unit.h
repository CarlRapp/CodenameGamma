#pragma once
#ifndef UNIT_H
#define UNIT_H

#include "../GameObject.h"
#include "../Weapons/Weapon.h"
#include "../Weapons/Pistol.h"

using namespace DirectX;

//	Is arranged like <Current, Max>
typedef pair<float, float>	UnitHealth;
static	float	MeterPerUnits	=	1 / 33.333f;

class Unit : public GameObject
{
protected:
	UnitHealth	gHealth;

	vector<Weapon*>	gWeaponList;
	Weapon*			gCurrentWeapon;

public:
	Unit(void);
	~Unit(void);

	void	Update(float deltaTime, Terrain* terrain);


	virtual void	SetTeam(GOTeam Value) { GameObject::SetTeam(Value); if ( gCurrentWeapon ) gCurrentWeapon->SetTeam(GetTeam()); }

	void			DropWeapon();
	void			SetWeapon(Weapon* Weapon);
	bool			HasWeapon() { return gCurrentWeapon != 0; }

	void			SetHealth(UnitHealth HealthData);
	UnitHealth		GetHealth();
	Weapon*			GetWeapon() { return gCurrentWeapon; }

	void	ReceiveDamage(float Damage);

	void	Hit(Unit* Target);

	void	ReloadWeapon();
	void	FireWeapon();

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
};

#endif