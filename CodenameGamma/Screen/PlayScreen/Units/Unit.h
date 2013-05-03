#pragma once
#ifndef UNIT_H
#define UNIT_H

#include "../GameObject.h"

using namespace DirectX;

//	Is arranged like <Current, Max>
typedef pair<float, float>	UnitHealth;

class Unit : public GameObject
{
private:
	UnitHealth	gHealth;

	float	gCooldown;

public:
	Unit(void);
	~Unit(void);

	bool	Update(float deltaTime, Terrain* terrain);

	void		SetHealth(UnitHealth HealthData);
	UnitHealth	GetHealth();

	void	ReceiveDamage(float Damage);

	void	Hit(Unit* Target);
};

#endif