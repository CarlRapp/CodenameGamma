#include "PlayerUnit.h"


PlayerUnit::PlayerUnit()
{
	gHunger	=	UnitHunger(5.0f, 100.0f);
	gThirst	=	UnitThirst(50.0f, 100.0f);
}

PlayerUnit::~PlayerUnit()
{
}

bool PlayerUnit::Update(float DeltaTime, Terrain* terrain)
{
	UpdateMeters(DeltaTime);

	return Unit::Update(DeltaTime, terrain);
}

void PlayerUnit::UpdateMeters(float DeltaTime)
{
	float	speed	=	GetSpeed();

	//	TODO
	//	Make the reduction
	//	speed based on
	//	how fast the unit
	//	is going.
	gHunger.first	-=	DeltaTime;
	gThirst.first	-=	DeltaTime;
}