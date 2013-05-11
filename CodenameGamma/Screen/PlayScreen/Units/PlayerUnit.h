#pragma once
#ifndef PLAYERUNIT_H
#define PLAYERUNIT_H

#include "Unit.h"

//	Is arranged like <Current, Max>
typedef pair<float, float>	UnitHunger;
typedef pair<float, float>	UnitThirst;

class PlayerUnit : public Unit
{
protected:
	UnitHunger	gHunger;
	UnitThirst	gThirst;


	void	UpdateMeters(float DeltaTime);
public:
	PlayerUnit(void);
	~PlayerUnit(void);

	bool	Update(float deltaTime, Terrain* terrain);

	UnitHunger	GetHungerMeter()	{ return gHunger; }
	UnitThirst	GetThirstMeter()	{ return gThirst; }

	void	Eat(float Value)
	{
		gHunger.first	+=	Value;
		gHunger.first	=	(gHunger.first > gHunger.second) ? gHunger.second : gHunger.first;
	}
	void	Drink(float Value)
	{
		gThirst.first	+=	Value;
		gThirst.first	=	(gThirst.first > gThirst.second) ? gThirst.second : gThirst.first;
	}

};

#endif