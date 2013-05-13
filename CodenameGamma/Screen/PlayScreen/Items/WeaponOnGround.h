#pragma once
#ifndef WEAPONONGROUND_H
#define WEAPONONGROUND_H

#include "Item.h"
#include "../Weapons/Weapon.h"
#include "../Units/Unit.h"
#include "../Graphics/LightHelper.h"

using namespace DirectX;

class WeaponOnGround : public Item
{
private:
	Weapon*		gWeapon;

	XMFLOAT3	gOffset;
	float		gTimeSpan;

	PointLight*	gPointLight;

public:
	WeaponOnGround(void);
	WeaponOnGround(Weapon* Instance);
	~WeaponOnGround(void);

	void	Update(float DeltaTime, Terrain* terrain);
	void	OnPickUp(Unit* Instance);
};

#endif