#pragma once
#ifndef PLAYERUNIT_H
#define PLAYERUNIT_H

#include "Unit.h"

//	Is arranged like <Current, Max>
typedef pair<float, float>	UnitHunger;
typedef pair<float, float>	UnitThirst;

class PlayerUnit : public Unit
{
private:
	UnitHunger	gHunger;
	UnitThirst	gThirst;

public:
	PlayerUnit(void);
	~PlayerUnit(void);

	bool	Update(float deltaTime, Terrain* terrain);

	UnitHunger	GetHungerMeter()	{ return gHunger; }
	UnitThirst	GetThirstMeter()	{ return gThirst; }


};

#endif