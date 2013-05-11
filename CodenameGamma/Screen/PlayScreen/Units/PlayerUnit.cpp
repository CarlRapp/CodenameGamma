#include "PlayerUnit.h"


PlayerUnit::PlayerUnit()
{
	gHunger	=	UnitHunger(100.0f, 100.0f);
	gThirst	=	UnitThirst(100.0f, 100.0f);
}

PlayerUnit::~PlayerUnit()
{
}

bool PlayerUnit::Update(float DeltaTime, Terrain* terrain)
{
	float	speed;
	XMStoreFloat(
		&speed,
		0.01f * XMVector3Length( XMLoadFloat3( &GetFloat3Value( Velocity ) ) )
	);

	gHunger.first	-=	DeltaTime * speed;
	gThirst.first	-=	DeltaTime * speed;

	return Unit::Update(DeltaTime, terrain);
}