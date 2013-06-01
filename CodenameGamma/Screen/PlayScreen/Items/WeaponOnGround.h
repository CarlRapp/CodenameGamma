#pragma once
#ifndef WEAPONONGROUND_H
#define WEAPONONGROUND_H

#include "Item.h"
#include "../Weapons/Weapon.h"

using namespace DirectX;

class WeaponOnGround : public Item
{
private:
	Weapon*		gWeapon;
protected:
	void	OnPickUp(Unit* Instance);

public:
	WeaponOnGround(void);
	WeaponOnGround(Weapon* Instance);
	~WeaponOnGround(void);

	void	Update(float DeltaTime, Terrain* terrain);
	//void	SetRotation(XMFLOAT4 Rotation);
};

#endif