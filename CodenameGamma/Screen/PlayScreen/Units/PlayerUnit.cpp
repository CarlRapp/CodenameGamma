#include "PlayerUnit.h"


PlayerUnit::PlayerUnit()
{
	gHealth	=	UnitHealth( 100.0f, 100.0f );
	gHunger	=	UnitHunger( 100.0f, 100.0f );
	gThirst	=	UnitThirst( 100.0f, 100.0f );
}

PlayerUnit::~PlayerUnit()
{
}

void PlayerUnit::Update(float DeltaTime, Terrain* terrain)
{
	UpdateMeters(DeltaTime);

	Unit::Update(DeltaTime, terrain);
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

	gHunger.first	=	( gHunger.first < 0 ) ? 0 : gHunger.first;
	gThirst.first	=	( gThirst.first < 0 ) ? 0 : gThirst.first;

	if ( gHunger.first == 0 )
		Hurt( 1.0f * DeltaTime );
	if ( gThirst.first == 0 )
		Hurt( 1.0f * DeltaTime );
}

void PlayerUnit::Hurt(float Damage)
{
	float	tDamage	=	Damage * ( 2 - gHunger.first * 0.01f );


	gHealth.first	-=	tDamage;
}

void PlayerUnit::SetVelocity(XMFLOAT3 Velocity)
{
	XMFLOAT3	newVelocity;
	float		tModifier	=	0.5f + gThirst.first * 0.01f;
	tModifier	=	MathHelper::Clamp( tModifier, 0.1f, 1.0f );

	XMStoreFloat3(
		&newVelocity,
		tModifier * XMLoadFloat3( &Velocity )
	);

	GameObject::SetVelocity( newVelocity );
}